---
layout: page
title: Lab 4
parent: Labs
grand_parent: EECS 4114 Embedded Systems
nav_exclude: true
---

# Lab 4: Timers

In this lab we will replace the dummy loops inserted in the previous lab to waste a certain amount of time for transitioning the traffic lights with more precise delays provided by the Xilinx AXI Timer core. The timer core contains two 32 bit timers that can be used independently or combined to form either (1) an extended 64 bit timer,
or (2) as a Pulse Modulation Wave generator. When used independently each timer can be set up in either (1) Capture, or (2) Generate Mode. There are many different solutions you could implement for this lab using a combination of the two timers. However the simplest solution is to use one timer in the Generate Mode. 

Before you enter the lab you should read and familiarize yourself with the Timer Core Datasheet. Specifically you should study the data sheet to determine:

1. How to set the control register to configure the timer to be in the generate mode, count down, automatically reload, and disable the generation of an external interrupt.
2. How to set the control register to transfer the value from the load register into the timer and then start the timer.
3. How to poll the status register to identify when the counter transitions down to 0 (i.e 3..2..1..0...reload new count from load register). Hint: You will have to poll a bit (read) and then once the transition is identified, clear the same bit by writing back a 1.

## Project Assignment

What follows next is a description of how to use a single timer in the Generate Mode to provide all timing for the Fayetteville Bike crossing. Here is a strategy:

1. Create a delay word to write into the load register to have the timer go off every .5 seconds.
2. When you enter the green light state, turn off the timer
3. When you enter the first flashing red state turn on the timer. You can use the timer going off every .5 seconds as your signal to alternate the red LED (flashing red light changes every .5 seconds). You will have to check for an additional button push which resets the sequence for another 6 seconds from the button push
4. In the solid red state the red light signal will remain on for six seconds. You can time this state by counting the number of .5 second timer events.
5. When you enter the second flashing red sequence the timer will be on and you can use the timer to once again alternate the flashing of the light every .5 seconds.

## Project Directions

To start off, we need to add the AXI Timer module to our SoC. This means we need to go back into Vivado and make some modifications to our block design. If you have kept your project organized such that you can modify your original design, then go ahead. If not, you can use the base-soc project that was given back in Lab 2 from here (https://github.com/zackheil/base-soc). It has instructions on how to get started with rebuilding the project from a set of TCL scripts. Note: if you have used this repository/project before, you might have an issue when opening Vivado and running the TCL script with an error saying the directory already exists. This is because the TCL script project is created in the AppData directory in Windows and not a set location like your Desktop or Documents folder. To get rid of this error, you can go to the Windows Start menu, type in %appdata% in the search bar and press enter. A directory called "Roaming" will open. From there, navigate to Xilinx\Vivado\ to see your temporary projects directory. Take the existing base-soc folder and rename it (e.g.: base-soc-OLD). Now you should be able to open Vivado and run the TCL script again.

Now that we have our Vivado project open, we need to go into the block design. Once in the block design, perform the following steps:

* On the left side of the block design diagram, there is a mini toolbar with quick actions. Click the "Add IP" button and search for the AXI Timer. Double click on it to add it to the block design.
* Run the automator from the blue pane that appears at the top. When choosing a clock source to connect to the timer, select the 83.3MHz "mig_7" source.
* Now, to future-proof the design with interrupt capability, let's add another IP block. Add the "AXI Interrupt Controller" to the design in the same fashion as the timer. Run the automation that appears in the banner.
* Now we need to manually connect certain I/O to respective ports to create functionality:
  * Connect the interrupt output on the left side of "AXI Interrupt Controller" to the "Interrupt" input on the right side of the MicroBlaze block.
  * Connect the interrupt output on the AXI Timer block to the intr[0:0] input on the AXI Interrupt Controller.
* Regenerate the layout and take a screenshot of your design. If you need help later on in the lab, I will need to see this layout to ensure you connected everything correctly.
* Now, just like Lab 1, validate the design, generate the HDL wrapper, and create the bitstream.
* Export the bitstream and launch SDK from the respective File menu options

> Note: if you do not choose to complete the lab in one sitting, and have your project stored in the AppData directory from the TCL loading process, remember **that is where your project is currently stored** when you return to work on the lab later.

Now, within SDK, create a new blank Application project for Lab 4. You are eventually going to transfer your FSM design of our bike-crossing light over and use the timer instead of the delay function previously created. Before you do this though, take some time to play around with the Timer module in C code either using the Xilinx API by including "xtmrctr.h" as a header file, or by using pointers like how you interface with GPIOs currently. The implementation choice is up to you. If you would like to use the "xtmrctr.h" APIs, you can open up the system.mss file included with your project and it will show the peripherals you currently have installed and C examples for how to interface with them with Xilinx APIs. Because this method of controlling the timer is vastly different than using pointers, I will only be able to aid in debugging with the pointer implementations. So if you do not make headway with the API or don't know where to start, choose the pointer method. The base implementation of the pointer method is detailed below:

* Locate the addresses for the necessary timer registers detailed in the data sheet in the same way you located the LED and switch registers from the previous labs.
* Create a simple C program that implements an LED register set. I recommend the 4 green LEDs just for
simplicity. The goal with this mini project is get the timer to function in 1 second intervals to ensure you have everything working before trying to add timers to your existing code base and getting confused.
  * Before adding any timer code, ensure you can blink an LED or implement a binary counter timer, like the example below:

```c
void main(void)
{
  // setup LED direction and variables...
  int count = 0;
  
  while(1)
  {
    count++;
    *ledDataReg = count;
    
    // reset the binary counter when it reaches 15.
    if(count >= 0xF) { count = 0; }
  }
}
```

  * You will notice if trying to run the above code that all LEDs will just look like they are lit and not counting at all. This is because the while loop is going super fast. So we can temporarily slow this down with an empty for loop somewhere in the while loop that looks like this: `for (int i = 0; i < 5000; i++);`. You should be able to see the individual LEDs blink now
  * Now let's setup the timer before that while loop. You should have read the data sheet on the particular registers, so it should be a simple task to set the necessary bits in the following registers:
    * For this sample code, we want the LEDs to count up every second. So we want the timer to count down from a certain value and when it hits 0, it will raise a flag. So, given the timer runs off the 83.33MHz clock, counting down from 83.33 million will take exactly one second to do. Set TLR0 to 83.33 million.
    * For TCSR0, we want to set a combination of bits in a certain order. First, set the mode to generate, set the timer to count down, we want to auto-reload the timer, and we want to load the TLR0 into the timer register. All of this can be set at once using a binary or hex number value, like this (values are not correct): `*TCSR0 = 0b1001001010`.
    * Now to enable the timer, the data sheet says that if we load a value into the register (set bit 5 TCSR0), we need to clear that bit while enabling the timer, else the timer will not start. So again, set the TCSR0 register, but clear bit 5 while also setting the enable timer bit. The timer will now count down from 83.33 million and will raise a flag on TCSR0 (bit 8) when it hits 0.
    * Finally, to use this in action, we need to detect when that flag is set in our loop and take appropriate action to clear the flag so we can detect when the next count down hits 0. Remove the for(...) loop that was created above to slow down the while loop. This for loop and our previously created delay_ms() function stalls the processor so it has to take a considerable amount of time processing pointless instructions when it could be executing other code in the while loop. Instead, we want to make the while loop as fast as possible and not stall it with a wait and instead just poll the timer and see when it completes and take action to reset it. Instead, in the place of the for loop, create an if statement that can detect the complete flag bit of the timer. Something like this would work: if((*TCSR0 & 0b00100000000) == 0b00100000000). Now if and only if we have a timer that counted down zero, we want to increment our count, so move the count++ into this if statement.
    * If you were to run this code at this point, it would not work. This is because you need to clear this flag bit once you discover it is set, else you would just read the flag with every following loop. To clear the bit, the data sheet says to set the bit to 1 (yes, it already is a 1 when you read it, but they made it reset when you write a 1 to it). Add this set bit operation to the if statement.
    * Run the code and observe how it counts up to 15 with each step taking one second
  * Now that you know how to interface with the timer, implement the above behavior using your FSM from last week. Replace the stalling delay_ms function with appropriate function calls that turn on, off, and read the timer complete flag

> Ask for clarification if you have any questions

## Pre-Lab Questions

Your answers should be typed and printed with your name, date and lab number. In addition to answering the following questions, please read the lab assignment on this page and reread Xilinx AXI Timer Datasheet. You may want to write pseudo code if you think you may have trouble finishing the project during your lab time or if you're feeling like a good student.

1. What is the advantage of a timer over the previously used delay_ms function?
2. There are several registers required to interface with the timer module in the SoC, what are those registers and what is their function?
3. When enabling the timer, there is one bit you need to set and one bit you need to clear. What are those two bits?
4. What two methods exist in C for setting the timer parameters?

## References

1. [Xilinx AXI Timer core](https://docs.xilinx.com/v/u/en-US/pg079-axi-timer)
