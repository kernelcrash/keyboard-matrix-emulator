keyboard-matrix-emulator
========================

Use a STM32F407 board to pretend to be a keyboard of an old computer that uses a
simple row/column matrix. You connect a PS2 port keyboard to the STM32F407 
board. Pressing a key on the PS2 keyboard ends up registering a keypress on 
the old computer. I made this for the cases where I have some computer mainboard,
but not the original keyboard. In this example case, its designed for a 
Galaksija computer which used a 7 column by 8 row keyboard matrix with the
Galaksija strobing each column low in sequence and reading the 8 row lines to
see if a key was pressed. It should be feasible to adapt this for other systems
that use 8x8 (or larger) key matrixes.

How it works
------------
- One part of the program is interrupt driven and is running off the PS2 
clock line to read keypresses on the keyboard.
- The keypress is turned into a row and column reference 
- some lookup tables determine which row line should be pulled low when
the relevant column is strobed low.
- The main loop is continually checking the columns strobes from the old 
computer. It immediately checks whether it needs to pull a row line 
low to indicate a key is pressed. The delay between detecting a column strobe
low and lodging a response on a row can be critical. I made something similar 
for an Oric Atmos using an Arduino Uno and that was not as fast I had hoped.
For this STM32F4 based thing (running at 168MHz, so not overclocked), I am seeing
between 200ns and 350ns response time which is not too bad for this use case.

Compiling/Installing
--------------------

This is a STM32CubeMX project targetting a STM32F407VGT6 board (it should work on other STM32F407 boards). Assuming you have the ARM compiler chain installed in
your PATH, you should be able to just 
```
   make
```
I just used dfu-util to install. Tie BOOT0 high. Plug it in to a USB port and
enter something like

sudo dfu-util -v -d 0483:df11 -a 0 -s 0x08000000 -D build/keyboard-matrix-emulator.bin

Then power off, Put BOOT0 to GND, plug in a PS2 keyboard, plug in the PD and PE ports to the old computer and power up.

Wiring
------

Wire the mini DIN PS2 socket to
```
  PC0 - PS2 clock pin
  PC1 - PS2 data pin
  +5V - +5V
  GND - GND
```

Wire the keyboard like so
-------------------------
This is specifically for the Galaksija by Fifan board

I've made some extra notes here as my circuit diagram for the Galaksija by
Fifan did not match the physical layout of the board
```
Columns
-------

XP2       74LS145 pin          STM32F407 pin
----      -----------          -------------

NC
p7        p7                   PD6
p6        p4                   PD3
p5        p6                   PD5
p4        p3                   PD2
p3        p5                   PD4
p2        p2                   PD1
p1        p1                   PD0

Rows
----

XP3       74LS251 pin          STM32F407 pin
----      -----------          -------------
p8        p1                   PE3
p7        p2                   PE2
p6        p3                   PE1
p5        p4                   PE0
p4        p15                  PE4
p3        p14                  PE5
p2        p13                  PE6
p1        p12                  PE7
```
And the Galaksija key matrix. It's a little unusual as far as
key matrix's go, as the keys are in somewhat alphabetic order,
rather than position order. I think this was due to limited size
of the original Galaksize ROM.

```

         col0  col1  col2  col3  col4  col5  col6

row0      NC    H     P     X     0     8     RET

row1      A     I     Q     Y     1     9     BRK

row2      B     J     R     Z     2     ;     RPT

row3      C     K     S     UP    3     :     DEF

row4      D     L     T     DOWN  4     ,     LIST

row5      E     M     U     LEFT  5     =     L.SHFT

row6      F     N     V     RIGHT 6     .     R.SHFT

row7      G     O     W     SPACE 7     /     NC

```




