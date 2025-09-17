---
layout: page
title: Lab 3
parent: Labs
grand_parent: Embedded Systems 2025
nav_exclude: true
---

# Lab 3: Fayetteville Bike Crossing

In this lab you will use your Arty-7 board and knowledge of push buttons and LEDs from the previous lab to create a controller for Fayetteville’s bike/waking paths. The bike/walking path crosses streets where cyclists/walkers have a control box they can use to control the traffic lights for cars

Cyclist/Walker control boxes: One on each side of the path that crosses the road. Cyclists/Walkers push the button in the box to request a crossing and control the traffic lights.

**Traffic Lights:**

1. Solid Green: Cars are free to travel through the crossing at speed. This signifies no cyclists/walkers have requested a crossing.
2. Solid Red: Cars must stop. This signifies a cyclist/walker is in the crosswalk.
3. Blinking Red: Cars must slow and look for cyclists/walkers. A blinking red occurs prior to and immediately following a solid red light.

__NB:__ A button press in the second flashing state should not reset the stage. 

**Operation and control of the push button control box:**

When a cyclist/walker pushes the button in the control box,

1. If the traffic lights are green they immediately start flashing red for 6 seconds
2. If the traffic lights are already flashing red the new button push will flash for an additional 6 seconds from the time of the new button push.
3. If the traffic lights are solid red, the button push will have no effect and the solid red will display for 4 seconds from the first button push.

So the states in order go: green → blinking red → solid red → blinking red → green. In general, any of the push buttons should activate the crosswalk logic, but all 4 RGB LEDs should be used for the status for the cars.

## Lab Directions

This is a two part lab that will run in two weeks. In the first week you will simulate the 4 and 6-second delays using dummy loops. In the second week you will replace the dummy loops with a timer.

- In the first week you will write the code to control two push buttons signifying the two cyclist/walkers control boxes and two LEDs signifying the traffic lights. After a control box is pushed, the traffic lights automatically transition between blinking red, solid red, blinking red, and then back to green. You will need to simulate this transition using delay loops. i.e., after a button is pushed the blinking red should appear for approximately 6 seconds, then switch to solid red for 4 seconds, back to blinking red for 6 seconds, and then back to green. Since you don’t have timers you will have to write nested loops that essentially do nothing useful but delay for the appropriate amount of time (approximately, can be a little fast or slow) before switching to the next color.

You will also have to blink the red LED on and off at approximately 1 second intervals to simulate the flashing red.

**Step 1:**

In Vitis, create a new blank Application project in the workspace like in last lab. Create a `lab3.c` file and populate it with the necessary empty main() function and includes.

1. To start off, we can see this lab is going to utilize waiting in varying time intervals based around the second unit. It would probably be helpful if we created a function to wait for us. Let's make a `delay_ms(int ms)` function, so we can reuse it in other portions of this lab and possible future labs:
    - Copy/paste the following to your source code file:

```c
void delay_ms(int ms) 
{ 
  for(int i = 0; i < 134 * ms; i++) 
  { 
    asm("nop"); 
  } 
}
```

*You may need to tweak the loop so that it approximates a second when delay_ms(1000) is called.*

Now that you have a working delay function, you can delay for one second by calling `delay_ms(1000);` in your main function. Create a `while(1)` section in your main function and create the desired functionality described above using what you have learned from last lab. It would be helpful to break the individual sub-routines down into functions for organization and modularity.

**Step 2:**

You will factor your code into a Finite State Machine model for creating state transitions. You should be familiar with FSM models from Digital Design and System Synthesis. At a high level, an FSM is an abstract machine or mathematical model that describes the transitions from a current state and set of inputs to a next state with a set of outputs. FSMs are very convenient for modeling and implementing the behaviors of embedded and real time systems as a series of transitions that result from the presence of external inputs (sensors, buttons) that result in the setting of external outputs (actuators, relays etc.). You should have some experience from Digital Design and System Synthesis in writing Moore and Mealy FSMs in Verilog or VHDL. We can model an FSM in C using the switch(), case 1, case 2... constructs.

Below is a skeleton template (not complete) of an FSM for the Fayetteville bike crossing.

```c
typedef enum 
{RED, GREEN, FLASH_RED_START, FLASH_RED_END} State; 

// Create state variables 
State state, next_state;

// Basic template for FSM model 
void FSM_tick() 
{ 
  switch(state) 
  { 
    case RED: 
      // code for RED state & logic for next_state
      next_state = ?; // next state after RED 
      break; 
    case GREEN: 
      // code for GREEN state & logic for next_state
      next_state = ?; // next state after GREEN
      break; 
    case FLASH_RED_START: 
      // code for FLASH_RED_START state & logic for next_state 
      next_state = ?; // next state after RED_START
      break; 
    case FLASH_RED_END: 
      // code for FLASH_RED_END state & logic for next_state 
      next_state = ?; // next state after FLASH_RED_END 
      break; 
  } 
  state = next_state; // update next state 
}

int main() 
{ 
  // initialize crossing to a state from the enum
  state = ?; 
  
  // keep on loopin' 
  while(1) 
  { 
    // cycle through traffic light pattern 
    FSM_tick(); 
  }

  return 0; 
}
```

{: .hint}
> - Your solution should not use any loops other than the main `while(1)` loop and the loops in the `delay_ms()` function.

## Submission

1. Submit your code file to the appropriate assignment on Blackboard.
2. No report is required for this week.

{: .note}
You should double-check your design and ensure it produces the desired behavior. Make sure your design is modular as we will refactor this code and use timers in place of the dummy delay loops in the next week. Next week's submission will require a combined lab report for this week and next week's lab.

## Prelab Assignment

Next week!
