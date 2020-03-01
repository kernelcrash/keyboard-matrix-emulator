/*
  PS2Keyboard.cpp - PS2Keyboard library
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Christian Weichel <info@32leaves.net>

  ** Modified for use beginning with Arduino 13 by L. Abraham Smith, <n3bah@microcompdesign.com> * 

  ** Modified to include: shift, alt, caps_lock, caps_lock light, and hot-plugging a kbd  *
  ** by Bill Oldfield 22/7/09 *
  
  ** Modified for Arduino 17 & brought clockPin into the library & added a keyboardLites blocker & added [;:] + ['"] -- couldn't get [\|] to work tho'
  ** by allen joslin 05/17/10

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "main.h"
#include <stdint.h>
//#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"


//#include "RawPS2Keyboard.h"

//#include "avr/io.h"
//#include "avr/interrupt.h"
//#include "avr/pgmspace.h"
//#include "binary.h"




/*
 * I do know this is so uncool, but I just don't see a way arround it
 * REALLY BAD STUFF AHEAD
 *
 * The variables are used for internal status management of the ISR. The're
 * not kept in the object instance because the ISR has to be as fast as anyhow
 * possible. So the overhead of a CPP method call is to be avoided.
 *
 * PLEASE DO NOT REFER TO THESE VARIABLES IN YOUR CODE AS THEY MIGHT VANISH SOME
 * HAPPY DAY.
 */

boolean ps2Keyboard_Lites;
int ps2Keyboard_ClockPin;
int  ps2Keyboard_DataPin;
unsigned int ps2Keyboard_CurrentBuffer;
volatile unsigned int ps2Keyboard_CharBuffer;
volatile byte ps2Keyboard_BufferPos;

// variables used to remember information about key presses
volatile boolean ps2Keyboard_shift;     // indicates shift key is pressed
volatile boolean ps2Keyboard_ctrl;      // indicates the ctrl key is pressed
volatile boolean ps2Keyboard_alt;       // indicates the alt key is pressed
volatile boolean ps2Keyboard_extend;    // remembers a keyboard extended char received
volatile boolean ps2Keyboard_release;   // distinguishes key presses from releases
volatile boolean ps2Keyboard_release_flag;
volatile boolean ps2Keyboard_caps_lock; // remembers shift lock has been pressed

// vairables used in sending command bytes to the keyboard, eg caps_lock light
volatile boolean cmd_in_progress;
volatile int     cmd_count;
         byte    cmd_value;
volatile byte    cmd_ack_value;
         byte    cmd_parity;
volatile boolean cmd_ack_byte_ok;

// sending command bytes to the keybaord needs proper parity (otherwise the keyboard
// just asks you to repeat the byte)
byte odd_parity(byte val) {
  int i, count = 1;  // start with 0 for even parity
  for (i=0; i<8; i++) {
    if (val&1) count++;
    val = val>>1;
  }
  return count & 1; // bottom bit of count is parity bit
}

void kbd_send_command(byte val) {
  // stop interrupt routine from receiving characters so that we can use it
  // to send a byte
  cmd_in_progress = TRUE;
  cmd_count       = 0;

  // set up the byte to shift out and initialise the ack bit
  cmd_value      = val;
  cmd_ack_value  = 1;    // the kbd will clear this bit on receiving the byte
  cmd_parity     = odd_parity(val);

  // set the data pin as an output, ready for driving
  //digitalWrite(ps2Keyboard_DataPin, HIGH);
  LL_GPIO_SetOutputPin(GPIOC, PS2_DATA_PIN);
  //pinMode(ps2Keyboard_DataPin, OUTPUT);
  LL_GPIO_SetPinMode(GPIOC, PS2_DATA_PIN, LL_GPIO_MODE_OUTPUT);


  // drive clock pin low - this is going to generate the first
  // interrupt of the shifting out process
  //pinMode(ps2Keyboard_ClockPin, OUTPUT);
  LL_GPIO_SetPinMode(GPIOC, PS2_CLOCK_PIN, LL_GPIO_MODE_OUTPUT);
  //digitalWrite(ps2Keyboard_ClockPin, LOW);
  LL_GPIO_ResetOutputPin(GPIOC, PS2_CLOCK_PIN);

  // wait at least one clock cycle (in case the kbd is mid transmission)
  //delayMicroseconds(60);
  LL_mDelay(1);


  // set up the 0 start bit
  //digitalWrite(ps2Keyboard_DataPin, LOW);
  LL_GPIO_ResetOutputPin(GPIOC, PS2_DATA_PIN);

  // let go of clock - the kbd takes over driving the clock from here
  //digitalWrite(ps2Keyboard_ClockPin, HIGH);
  LL_GPIO_SetOutputPin(GPIOC, PS2_CLOCK_PIN);

  //pinMode(ps2Keyboard_ClockPin, INPUT);
  LL_GPIO_SetPinMode(GPIOC, PS2_CLOCK_PIN, LL_GPIO_MODE_INPUT);

  // wait for interrupt routine to shift out byte, parity and receive ack bit
  while (cmd_ack_value!=0) ;

  // switch back to the interrupt routine receiving characters from the kbd
  cmd_in_progress = FALSE;
}

void mbd_reset() {
  kbd_send_command(0xFF);   // send the kbd reset code to the kbd: 3 lights
                            // should flash briefly on the kbd

  // reset all the global variables
  ps2Keyboard_CurrentBuffer = 0;
  ps2Keyboard_CharBuffer    = 0;
  ps2Keyboard_BufferPos     = 0;
  ps2Keyboard_shift         = FALSE;
  ps2Keyboard_ctrl          = FALSE;
  ps2Keyboard_alt           = FALSE;
  ps2Keyboard_extend        = FALSE;
  ps2Keyboard_release       = FALSE;
  ps2Keyboard_release_flag = FALSE;
  ps2Keyboard_caps_lock     = FALSE;
  cmd_in_progress           = FALSE;
  cmd_count                 = 0;
  cmd_value                 = 0;
  cmd_ack_value             = 1;
}

// val : bit_2=caps_lock, bit_1=num_lock, bit_0=scroll_lock
void kbd_set_lights(byte val) {
  if (ps2Keyboard_Lites) {
        // When setting the lights with the 0xED command the keyboard responds
        // with an "ack byte", 0xFA. This is NOT the same as the "ack bit" that
        // follows the succesful shifting of each command byte. See this web
        // page for a good description of all this:
        // http://www.beyondlogic.org/keyboard/keybrd.htm
    cmd_ack_byte_ok = FALSE;   // initialise the ack byte flag
    kbd_send_command(0xED);    // send the command byte
    while (!cmd_ack_byte_ok) ; // ack byte from keyboard sets this flag
    kbd_send_command(val);     // now send the data
  }
}

// The ISR for the external interrupt
// This may look like a lot of code for an Interrupt routine, but the switch
// statements are fast and the path through the routine is only ever a few
// simple lines of code.
void ps2interrupt (void) {
  //int value = digitalRead(ps2Keyboard_DataPin);
  int value  = ( ( LL_GPIO_ReadInputPort(GPIOC) >> 1) & 0x0001);

  // This is the code to send a byte to the keyboard. Actually its 12 bits:
  // a start bit, 8 data bits, 1 parity, 1 stop bit, 1 ack bit (from the kbd)
  if (cmd_in_progress) {
    cmd_count++;          // cmd_count keeps track of the shifting
    switch (cmd_count) {
    case 1: // start bit
      //digitalWrite(ps2Keyboard_DataPin,LOW);
      LL_GPIO_ResetOutputPin(GPIOC, PS2_DATA_PIN);
      break;
    case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9:
      // data bits to shift
      //digitalWrite(ps2Keyboard_DataPin,cmd_value&1);
      LL_GPIO_WriteOutputPort(GPIOC, ((cmd_value & 1) << 1) );
      cmd_value = cmd_value>>1;
      break;
    case 10:  // parity bit
      //digitalWrite(ps2Keyboard_DataPin,cmd_parity);
      LL_GPIO_WriteOutputPort(GPIOC, ((cmd_parity & 1) << 1) );
      break;
    case 11:  // stop bit
      // release the data pin, so stop bit actually relies on pull-up
      // but this ensures the data pin is ready to be driven by the kbd for
      // for the next bit.
      //digitalWrite(ps2Keyboard_DataPin, HIGH);
      LL_GPIO_SetOutputPin(GPIOC, PS2_DATA_PIN);
      //pinMode(ps2Keyboard_DataPin, INPUT);
      LL_GPIO_SetPinMode(GPIOC, PS2_DATA_PIN, LL_GPIO_MODE_INPUT);
      break;
    case 12: // ack bit - driven by the kbd, so we read its value
      //cmd_ack_value = digitalRead(ps2Keyboard_DataPin);
      cmd_ack_value  = ( ( LL_GPIO_ReadInputPort(GPIOC) >> 1) & 0x0001);
      cmd_in_progress = FALSE;  // done shifting out
    }
    return; // don't fall through to the receive section of the ISR
  }

  // receive section of the ISR
  // shift the bits in
  if(ps2Keyboard_BufferPos > 0 && ps2Keyboard_BufferPos < 11) {
    ps2Keyboard_CurrentBuffer |= (value << (ps2Keyboard_BufferPos - 1));
  }
  ps2Keyboard_BufferPos++; // keep track of shift-in position

  if(ps2Keyboard_BufferPos == 11) { // a complete character received
    ps2Keyboard_CharBuffer = ps2Keyboard_CurrentBuffer;

    ps2Keyboard_CurrentBuffer = 0;
    ps2Keyboard_BufferPos = 0;
  }
}

void kbd_begin(int dataPin, int clockPin, boolean kybdLites) {
  // Prepare the global variables
  ps2Keyboard_Lites         = kybdLites;
  ps2Keyboard_DataPin       = dataPin;
  ps2Keyboard_ClockPin      = clockPin;
  ps2Keyboard_CurrentBuffer = 0;
  ps2Keyboard_CharBuffer    = 0;
  ps2Keyboard_BufferPos     = 0;
  ps2Keyboard_shift         = FALSE;
  ps2Keyboard_ctrl          = FALSE;
  ps2Keyboard_alt           = FALSE;
  ps2Keyboard_extend        = FALSE;
  ps2Keyboard_release       = FALSE;

  ps2Keyboard_caps_lock     = FALSE;
  cmd_in_progress           = FALSE;
  cmd_count                 = 0;
  cmd_value                 = 0;
  cmd_ack_value             = 1;

  // initialize the pins
  //pinMode(ps2Keyboard_ClockPin, INPUT);
  LL_GPIO_SetPinMode(GPIOC, PS2_CLOCK_PIN, LL_GPIO_MODE_INPUT);
  //digitalWrite(ps2Keyboard_ClockPin, HIGH);
  LL_GPIO_SetOutputPin(GPIOC, PS2_CLOCK_PIN);
  //pinMode(dataPin, INPUT);
  LL_GPIO_SetPinMode(GPIOC, PS2_DATA_PIN, LL_GPIO_MODE_INPUT);
  //digitalWrite(dataPin, HIGH);
  LL_GPIO_SetOutputPin(GPIOC, PS2_DATA_PIN);

  //attachInterrupt(1, ps2interrupt, FALLING);
#if 0
  // Global Enable INT1 interrupt
  EIMSK |= ( 1 << INT1);
  // Falling edge triggers interrupt
  EICRA |= (0 << ISC10) | (1 << ISC11);
#endif
}

boolean kbd_available() {
  return ps2Keyboard_CharBuffer != 0;
}

// This routine allows a calling program to see if other other keys are held
// down when a character is received: ie <ctrl>, <alt>, <shift> or <shift_lock>
// Note that this routine must be called after available() has returned true,
// but BEFORE read(). The read() routine clears the buffer and allows another
// character to be received so these bits can change anytime after the read().
byte kbd_read_extra() {
  return (ps2Keyboard_caps_lock<<3) |
         (ps2Keyboard_shift<<2) |
         (ps2Keyboard_alt<<1) |
          ps2Keyboard_ctrl;
}

byte kbd_read() {
  byte result;

  // read the raw data from the keyboard
  result = ps2Keyboard_CharBuffer;

  ps2Keyboard_CharBuffer = 0;

  return(result);
}
