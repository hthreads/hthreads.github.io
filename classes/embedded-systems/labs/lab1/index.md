---
layout: page
title: Lab 1
parent: Lab Repository
grand_parent: Embedded Systems
nav_exclude: true
---

## Introduction

## Prelab Questions

1. What is an FPGA?
2. What is a system-on-chip? Why is it different from your desktop computer system?
3. What does soft-core IP mean?
4. What does it mean to cross-compile a program?
5. Why does it take so long to configure/build the Hardware portion of your system on the FPGA?
6. How does the desktop computer program the FPGA, how does it monitor the FPGA?


## Description
In laboratory 1 you will complete a provided boilerplate design of a system-on-chip (SoC) using the Xilinx-Vivado development suite and targeted at the [Arty-A7-35](https://www.xilinx.com/products/boards-and-kits/1-elhaap.html) development board. Your design will contain a MicroBlaze soft-processor, a serial port, and three GPIO devices that interface with components on the Arty board. The processor will be connected to the serial port and GPIO peripherals in a shared-bus architecture. You will then program this SoC using the C programming language, to print out your name and student ID five times to the screen of a desktop computer which is connected to the Arty-7 development board using the serial port. There will be 3 screenshot requirements for your report, so ensure you read the following instructions carefully.
Project Assignment

    Use the provided base project to create a basic system-on-chip.
    Synthesize your system-on-chip, compile your software project, and combine them into a bitstream.
    Download your bitstream onto the Arty-A7-35 development boardOpen this document with ReadSpeaker docReader  and execute it.
    Create a software project and make it print out your name five times.

This may not seem like much, but at the end of this laboratory you will have created a system-on-chip and executed a program on top of it in a bare metal environment. This represents a very basic but fully functional embedded system.

