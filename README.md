# MPT2-Project_LEDCUBE

Project prepared for the purpose of MPT2 Laboratories.

Authors: Aleksandra Macura & Kacper Godula

Video presentation: https://youtu.be/7ULbOOuEuOo

# Introduction

As our project we prepared RGB Led cube 8x8x8. Our main goal was to create someinteresting games and nice looking animations. In the end our project consists of 2 games -snake and tetris, which can be controlled either by computer keyboard via PuTTY or in theapp on the phone using Bluetooth. In this report we are going to briefly describe thehardware part of our project as well as the individual files of the code part.


![obraz](https://user-images.githubusercontent.com/45372078/105532763-8eee8b80-5ceb-11eb-9625-d26315260ab2.png)


# Hardware

* The Cube

It consists of 512 RGB diodes with common anode in layer. The cube is standing on aprinted circuit board, made at home using thermal transfer overprinting method - printing thedesired circuit from KiCad, ironing it to the plate and then etching the rest of the copperusing Iron(III) chloride. On the board there are soldered 24 SCT2167 integrated circuitsbecause of which we are able to send data to our LEDs using the limited amount of pinsfrom the processor. Beside there are also power amplifiers for each layer of the cubeconsisting of one NPN transistor, one MOSFET and 3 resistors.

* Freescale Kinetis KL05Z

As the base of our project we used the same board as during laboratory classes. This waywe already had experience in using the useful things like PWM, Systick, UART  on exactlythe same hardware

* Bluetooth module

We used HC-06. Because of this device we are able to connect to the app on the phone.

* Keyboard

As we didn’t have enough pins to connect it whole(4x4) we used only a few buttons, but it is enough for our needs because in this version of our project we use it only as a device toswitch between the games.


# Software

![vlcsnap-2021-01-20-19h41m55s189](https://user-images.githubusercontent.com/45372078/105534194-6bc4db80-5ced-11eb-8c0d-6d028db5a943.png)

We wrote our whole project in C language using the Keil uVision5 IDE.Our code consists of the following files:

* main.c

In this file you can find the most important part, without which the program wouldn’t work -eg. Enable clock, interrupts as well as the base of the game - choosing which game we wantto play after leaving the demo part. There is also written the mechanics of the way that theKeyboard via UART works.

* tpm.c

It is responsible for controlling the brightness of each of the 3 colors. We are using TPM0(channels 0, 1 and 3) with modulo value set to 65 in order to get period ~0,2ms - 10 timessmaller than SysTick.

* sct.c

In this file we have only one but very important function - Send8BytesT. It is responsible forsending data  to the integrated circuits. In every iteration of the for loop in this function wesend 1 byte for each color which is later moved further.

* systick.c

Here we are dealing with SysTick_Handler. We are checking whether buttons on thephysical keyboard were pressed and also changing currently active layer. We also use theSent8BytesT function to send data. The last part is handling pressing the buttons on thekeyboard.

* uart0.c

In this file we are handling the communication via UART because of which our games canbe controlled using Bluetooth or PuTTY on the PC. In the UART0_IRQHandler we enableeverything that we need and in UART0_IRQHandler we manage the symbols assigned to allof the buttons that we can press either in the app on the phone or on the keyboard of thecomputer.

* tetris.c

In the beginning we have declaration of the blocks. We can declare blocks of maximum sizeof 3x3. In that matrix we declare which diodes we want to turn on by assigning “1” to them.This way we create a number which “describes” the block. Later we say what are thedimensions of the block - without it there were problems with moving the blocks to the edgeof the cube. In the next parts of the file we have all of the functions thanks to which playingtetris is possible, for example Draw_Block, Move_Block or uint8_t Tetris(void) which is thefunction for the main operation of the game. The whole game works as regular tetris - fromthe top of the cube blocks are falling randomly until they stop on the bottom level or onanother block. When the whole layer is full then it disappears and when the block stops onthe top level the game ends.

* snake.c

In this part of code we prepared the second game - snake. The main idea is the same as inthe original game - the more apples the snake eats, the longer it becomes. The game endswhen the snake hits the wall of the cube or itself. Then the result is shown on the first wall ofthe cube. There are functions  responsible for all of the things that are needed for the snake- appearing of the snake, appearing of apple, eating an apple, moving the snake and endingthe game.

* demo.c

In this file there are prepared functions with animations. Some of them are really simple, like lighting up all of the diodes of one color or some specific shape, but there is alsodemo_obroty which lights up the contour of the cube and then rotates it.

All of the .c files have its header files. This way the code is more readable and easy to change.

----

Appendix 1: schematics_led_cube.pdf <br/>
Appendix 2: Led_cube_bottom.pdf
