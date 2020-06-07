keyboard-matrix-emulator
========================

Use a STM32F407 board to pretend to be a keyboard of an old computer that uses a
simple row/column matrix. You connect a PS2 port keyboard to the STM32F407 
board. Pressing a key on the PS2 keyboard ends up registering a keypress on 
the old computer. I made this for the cases where I have some computer mainboard,
but not the original keyboard. In this example case, its designed for a 
Omega MSX2 computer. This computer outputs a binary row on 4 pins, and
then reads back the column on 8 data pins.

How it works
------------
- One part of the program is interrupt driven and is running off the PS2 
  clock line to read keypresses on the keyboard.
- The keypress is turned into a row and column reference 
- some lookup tables determine which row line should be pulled low when
  the relevant column is strobed low.
- The key thing is that we need to closely monitor the 4 bit row
  output coming from the host computer. When that changes we need to
  immediately change the column output pins. If we delay much at all
  here, then its likely the host computer will register a false 
  keypress. So we do this mainly by generating interrupts on all 4
  row pins interrupting on both rising and falling edges to pick up
  all changes.


Compiling/Installing
--------------------

This is a STM32CubeMX project targetting a STM32F407VGT6 board (it should work on other STM32F407 boards). Assuming you have the ARM compiler chain installed in
your PATH, you should be able to just 
```
   make
```
I just used dfu-util to install. Tie BOOT0 high. Plug it in to a USB port and
do an lsusb. You should see something like

Bus 003 Device 078: ID 0483:df11 STMicroelectronics STM Device in DFU Mode

enter something like

sudo dfu-util -v -d 0483:df11 -a 0 -s 0x08000000 -D build/keyboard-matrix-emulator.bin

Then power off, Put BOOT0 to GND, plug in a PS2 keyboard, plug in the PD and PE ports to the old computer and power up.

Wiring
------

Wire the mini DIN PS2 socket to
```
  PC4 - PS2 clock pin
  PC5 - PS2 data pin
  +5V - +5V
  GND - GND
```

Wire the keyboard like so
-------------------------
This is specifically for the Omega MSX2 computer

```
Row
---

ROW_A		PD0
ROW_B		PD1
ROW_C		PD2
ROW_D		PD3

Column
------

COLUMN_0	PE0	
COLUMN_1	PE1	
COLUMN_2	PE2	
COLUMN_3	PE3	
COLUMN_4	PE4	
COLUMN_5	PE5	
COLUMN_6	PE6	
COLUMN_7	PE7	





