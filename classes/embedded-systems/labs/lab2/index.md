---
layout: page
title: Lab 2
parent: Labs
grand_parent: CSCE 4114 Embedded Systems
nav_exclude: true
---

## Instructions
In lab 2, you will utilize the SoC created in lab 1 to program the MicroBlaze soft-processor to interface with several of the [GPIO (general purpose input/output)](/_modules/csce-4114/data-sheets/axi-gpio.pdf) peripherals built into the Arty development board. Unlike lab 1, all of the synthesis, implementation, and bitstream creation has been done before-hand and this lab will focus mainly on the C programming aspect of controlling an embedded system. There is some boilerplate code supplied that, when a push button is pressed, it will light the corresponding LED on the opposite side of the board. This session you will tinker with this code and observe how the pointers created within it relate to the memory map displayed in the  system.hdf file. From there, you will build on the supplied code to add support for the 4 switches on the Arty board and make them control the corresponding RGB LEDs. In order to complete this laboratory successfully, you will need to understand the workings of the GPIO peripheral by reading the  GPIO reference manualOpen this document with ReadSpeaker docReader .
### Project Assignment
- Create a new application project within SDK to upload to the Arty development board:
  - Copy and paste the demo C file project into the project
  - Load the demo code onto the Arty to observe the pointers in action 
- Add to the existing code to create the following functionality:
  - When SW0 (far right) switch is in the up position, LD0 (far right RGB LED) will glow white.
  - When SW1 switch is in the up position, LD1 will glow white.
  - When SW2 switch is in the up position, LD2 will glow white.
  - When SW3 (far left) switch is in the up position, LD3 (far left) will glow white.
  - When the respective switch is down position, the respective LED will turn off.
  - Just like the buttons in the demo code, when the status of the switches changes, print to the console the current state of the switch register. 
- Please allocate enough time outside of class to read and understand the project and the workings of the GPIO.

