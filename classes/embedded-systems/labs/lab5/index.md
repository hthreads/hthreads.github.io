---
layout: page
title: Lab 5
parent: Lab Repository
grand_parent: CSCE 4114 Embedded Systems
nav_exclude: true
---

# Lab 5: Interrupt Driven MicroBlaze System

The goal of these labs is to become familiar with the idea of interrupt-based processing techniques using the MicroBlaze processor. A base system will be built that utilizes an interrupt controller to allow for multiple interrupt sources along with a set of interrupt sources. The interrupt sources will include a timer as well as a software interrupt. After understanding what an interrupt does and how to properly implement them, we will then return to the bike trail crossing project and modify the design to use interrupts. You covered interrupts in class exactly as they work with the MicroBlaze. 

You can refer to Dr. Andrews' slides from lecture if you have conceptual misunderstandings.

## Lab Directions

For this lab, you will explore the use of software interrupts and timer interrupts in example programs and then you will modify the design of the bike trail traffic crossing signal to rely on interrupts rather than polling to see if the timer has completed.

To keep the design consistent between all students for grading purposes, the final design should contain/do the
following:

* Display a green (RGB) output on the display when no bikers or walkers are trying to cross the street.
* Display a flashing red output on the display immediately after any of the tactile push buttons are pressed.
  * This stage should last a total of 6 seconds
  * The amount of time the LED should be on or off in the flashing stage should be 0.5 seconds. (e.g.: 0.5s on, 0.5s off, 0.5s on, ...)
  * If the button is pressed during this stage, the 6 second timer should reset back to zero and the LEDs should blink for another 6 seconds. This is the only stage where this happens
* After the LEDs have flashed red for 6 seconds, the traffic signal should display the red LEDs for exactly 4 seconds
* After the solid red stage, the red LEDs should then flash red again for 6 seconds. A button press here should not affect nor reset any stage countdown timer.

## Background

The MicroBlaze processor has only a single interrupt port, called the Interrupt port. This port is a 1-bit signal that can be configured to be either edge- or level-triggered. A multiplexer must be used in order to create a MicroBlazebased system that has multiple interrupt sources. A multiplexer built specifically to handle interrupt sources is often referred to as an interrupt controller, or often, a programmable interrupt controller (PIC). The following diagram demonstrates how an interrupt controller is used to multiplex many interrupt sources onto a single
interrupt request line.

Interrupt.jpg

In this type of situation the processor must be able to determine which interrupts have fired and need servicing.
Most interrupt controllers contains registers that allow a processor to query the following:

* Which interrupts are enabled?
  * Interrupt Enable Register (IER).
* Which interrupts have fired and need servicing?
  * Interrupt Service Register (ISR).
* Which interrupts need to be cleared?
  * Interrupt Acknowledge Register (IAR).

When the processor receives an interrupt, it must first look at the interrupt service register (ISR) to figure out which interrupts need to be serviced. Each bit in the ISR represents an individual interrupt source. If a bit X in the ISR is '1' then interrupt source X needs to be serviced. If bit X in the ISR is '0' then the interrupt has not fired and does not need to be serviced.

After servicing each active interrupt, the processor must then clear the interrupt as well as its source. The ordering of this operation is extremely important due to the fact that clearing an interrupt before clearing its source will result in apparent multiple interrupt events for a single source event. Therefore it is pertinent to always clear an interrupt at its source, and then proceed until you have cleared the interrupt at the PIC and at the processor.

If interrupts are cleared incorrectly then “bad” situations can occur. For example:

1. Interrupt0 occurs.
    * Bit0 in the ISR is set to 1.
2. CPU receives an interrupt and then CPU jumps into its interrupt handler routine.
3. CPU checks Bit0 in the ISR.
    * CPU then services the interrupt, but forgets to clear Bit 0!!!!
4. The CPU now returns from its interrupt handler routine to continue “normal” processing.
5. OH NO, BUT WAIT!!!! Bit0 has never been cleared, so the CPU's interrupt line is still asserted!!!!
    * The CPU jumps right back into the interrupt handler routine, even though no new interrupts have occurred!!!!

In order to correct the situation described above, the interrupt handler should clear the interrupt at its source, and then at the PIC in step (3). Clearing an interrupt at the PIC can be done by writing a '1' to the bit of interest to the PIC's interrupt acknowledge register (IAR).

## Project Assignment

**Before Executing the Lab:**

I recommend looking through the AXI Timer [Documentation](https://www.xilinx.com/support/documentation/ip_documentation/axi_timer/v2_0/pg079-axi-timer.pdf) and AXI Interrupt Controller [Documentation](https://www.xilinx.com/support/documentation/ip_documentation/axi_intc/v4_1/pg099-axiintc.pdf) documents at their respective links to better understand the modules we will be working with in this lab. It is also worth mentioning that C documentation for interacting with these modules is available within SDK by looking at the system.mss file typically located within a project with the "_bsp" suffix.

**Hardware:**

In lab 4, we made modifications to the block design in Vivado to enable future interrupt capability; however, there was a slight oversight in the instructions of that design. Because this is a complex lab with a lot of new C code to comprehend, I won't make the modification of the block design a part of this lab and will supply a working bitstream file and example set of projects within a SDK workspace folder. This folder can be found here (https://github.com/zackheil/teaching-CSCE4114-EmbeddedSystems). To use the project, download the linked repository and extract the folder "lab5.sdk" to a usable space on your computer. From here, open SDK 2016.4, and when the prompt appears, choose the lab5.sdk folder to be the workspace location. This should open the SDK project with the included bitstream that supports interrupt functionality as well as a few example projects.

**Software:**

Now, you must write software to enable the features of the newly created interrupt-based system. This involves writing initialization code that will enable interrupt-related functions on all of the interrupt sources, the interrupt controller, and the MicroBlaze processor. Additionally, an interrupt handler routine will be written to handle interrupts when they occur. Conceptually, an interrupt handler routine can be thought of as a function that can be called at anytime from anywhere (if interrupts are enabled). This implies that an interrupt handler routine must not have any effect on the state of the CPU, otherwise, program behavior could be seriously altered by the assertion of an interrupt

An Example: Consider a program controlling a stoplight puts a counter value in r20 to represent how long to keep the yellow light lit, and let us also say that a “rogue” interrupt handler routine always sets r20 to 0x0000_0000. The program would not be able to detect when this happens due to the fact that an ISR can be invoked at any time interrupts are enabled. This would essentially cause the stoplight to go from green to red instantly, which could result in adverse real-world effects.

On the other hand, an interrupt handler routine is allowed to change values in memory (i.e. variables, device registers, etc.) as this does not have any side-effects on the actual state of the CPU. Thus, an interrupt handler routine must save all pertinent CPU state upon an interrupt, and must restore the CPU state after servicing needed interrupts. In the case of the MicroBlaze processor, this is handled by the “main” interrupt handler routine; which is a function that “wraps” up the user’s interrupt handler routine in such a way that the user does not have to worry about saving CPU state. This requires a programmer to “register” their interrupt handler routine so that the “main” interrupt handler knows which user-level handler to call upon invocation of an interrupt. Essentially, registration just involves putting the address of the user-level function in a place known by the “main” interrupt handler. This place is known as the interrupt vector table.

Interrupts on the MicroBlaze can be enabled/disabled by interacting with the MSR register. The MSR register is a special purpose register, so the MTS and MFS instructions are needed to read/write the MSR register.

The basic order of operations for your software should be the following:

* **Main program:**
  * Setup interrupts
  * Register a handler
  * Enable interrupts on all sources.
  * Setup the timer to generate a periodic interrupt.
    * This is done by setting bits in the timer Control and Status Register
      * In our case there a few hints I will give you, but the rest have to be understood and set by you
        * The timer mode should be set to generate
        * Enable external generate signal and enable external capture should be set to 0
        * For the counter to run load should be a zero, for the counter to Load a value it shouldn't be running
        * Pulse width modulation should be set to a zero
  * Enable interrupts on the MicroBlaze.
  * Enter an infinite loop to print out a global variable, called X.
 
* **Interrupt handler:**
  * Service each interrupt that has fired.
  * Do this by incrementing the global variable X.
  * Clear and service all interrupts that have fired.
  * Clear them at the source, then the PIC. 
  * Return

It's worth noting that all of these steps are required to get interrupts working from the ground up, but some of the functions provided by Xilinx that we will be using actually perform these operations behind the scenes. As you look through some of the examples in the provided SDK project, feel free to explore the source code of the provided functions by right clicking on functions, variables, or definitions and selecting "Open Declaration" to see where these calls are actually made.

## Project Instructions

* Start off by downloading and extracting the lab5.sdk folder from the Github link above.
* Open SDK and set the workspace to the extracted folder to get access to the example projects and the bitstream.
* From here, load the bitstream on to the Arty board. Before moving on to timer interrupts, let's look how we can trigger an interrupt using software with a provided example.
  * Open up the project "xintc_low_level_example_1" from the project explorer pane and locate the C file in the src directory. Look through the code and inspect what it will be doing before attempting to run it.
  * Open up a serial terminal monitor to the correct COM port like in earlier labs.
  * In the Project explorer, highlight the current project's root folder (has "xintc_low_level_example_1" in it) and then click on the debug icon dropdown in the toolbar beside the usual run button that is pressed to execute code on the Arty. Choose "debug as" and "run on system hardware". The code will stop as soon as it enters the main function.
  * Step through the code to see what each step is doing and how it operates. At some point in execution, you will see a function call to XIntc_SimulateIntr which will set the ISR register and will seemingly reroute the execution to the DeviceDriverHandler function at the bottom.
  * This function will write to the console that an interrupt has occurred and then will resume execution where it left off.
* Now that we have seen an example of how the ISR can be set with the simulate interrupt function that Xilinx has, let's now use the timer to trigger the interrupt. Open up the "timer_interrupt_example_zheil" project. This demonstrates a useful example where the Microblaze is just trying to execute a while loop where it outputs a counter value to the LEDs, yet is constantly interrupted to update the counter that the LED is outputting.
* Observe the extra steps to set up the timer interrupt and the options that were previously set using direct register setting operations with pointers.
* Also observe how the functions for setting up both the interrupt controller and the timer were extracted into the main function in a linear format so that the instructions could be seen without the need to jump into the function calls. In your implementation of the timer interrupts, take these operations and group them into meaningful function calls so your code isn't as linear as this demonstration.
* Highlight the project root in the project explorer and run this example. Note that it is a simple binary counter that increments every half second.
* Take some time to really understand this code, then create a new project and add the FSM design, but modify the design to use the interrupts instead of relying on polling. Remember that there are stricter grading guidelines for this lab, so be mindful of the timing for each state.


## Pre-Lab Questions

* Instead of pre-lab questions this week, I encourage you to please read through this entire lab assignment once and go through the example (commented) programs supplied in the lab5.sdk project.


## References

* AXI Timer Documentation (https://www.xilinx.com/support/documentation/ip_documentation/axi_timer/v2_0/pg079-axi-timer.pdf)
* AXI Interrupt Controller Documentation
(https://www.xilinx.com/support/documentation/ip_documentation/axi_intc/v4_1/pg099-axi-intc.pdf)
* File:Xps timer.pdf - XPS Timer documentation
* File:Edk interrupts.pdf - Step-by-step instructions on how to create interrupt-based systems (old version but go for it if you'd like)
