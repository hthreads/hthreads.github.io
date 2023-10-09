---
layout: page
title: Lab 6
parent: Labs
grand_parent: EECS 4114 Embedded Systems
nav_exclude: true
---

# Lab 6: Bare Metal Task Control Blocks (TCBs)

_--An Introduction to FreeRTOS-like Programming:_

The goal of this lab is to become familiar with the concept of pseudo-multitasking on microprocessors (which
typically only have a single core) using a stripped down version of a real-time operating system, or RTOS. You will
use the MicroBlaze system from past labs that includes timer interrupts, and a structure to organize the tasks we
want to process, called a task control block. These components form the basis for a non-preemptive round-robin
RTOS. This RTOS will be adapted for our Fayetteville bike trail crossing traffic signal to work with this paradigm.

## Lab Directions

For this lab we will be using the same SoC design from previous labs. The code you will be running can be found here File:Tasks.c.

## Project Instructions

Start off with your Lab 5 project. That project has all the necessary working components to get our task scheduler working. Your job for this lab is to study how your original code was restructured to form a round-robin scheduler. The following section outlines the highlights. It is important that you understand how the FSMs of the previous labs were broken up into scheduled tasks that are then managed by the simple task scheduler.

{: .note}
The scheduler is kicked off by the timer at a set interval. The Linux scheduler calls this a jiffy, while other RTOS's refer to this as a timer tick. The code provided in this lab will form the basis for the code you will develop on your own next week using FreeRTOS&trade;.

## Understanding the code

1. The body of `main()` puts all the Interrupt Controller and Timer module related function calls in a function called `init()`.
2. The timer is modified such that it fires an interrupt every 5ms.
3. A struct serving as a basic task control block (TCB) holds the following information:
    - `taskPtr` - this is a function pointer. It has a void return type and a void pointer parameter it can accept.
    - `taskDataPtr` - A void pointer that can be used to pass the function pointer as a parameter.
    - `taskReady` - Implemented as a boolean or a char. This determines if the scheduler will run our task or skip over it.
4. Now, because we are going to be translating our FSM-based implementation over to the RTOS we are building, we create functions that equate the actions of our FSM. There are several ways to go about doing this. For example, we could have one task manage the FSM logic, another task manage the RGB LED display, and another interface with the timer, giving us 3 tasks. But to provide us with a bit more variety, 5 tasks are created for this lab:
    - `taskChoose` - This is our first and highest priority task that determines which of the rest of the tasks need to be marked as ready or not. This is accomplished with a `switch` statement that evaluates the current state and sets the necessary `taskReady` flag for the following states...
    - `taskGreen` - Contains the green state related code.
    - `taskRedBlinkStart` - Contains the first blinking state related code.
    - `taskRedSolid` - Contains the solid red state related code.
    - `taskRedBlinkEnd` - Contains the ending blinking state related code.
5. Now that it is established that we have 5 tasks, a global array of 5 task (TCB) pointers is created.
6. Inside `main()`, prior to the infinite loop, we need to allocate memory for these pointers (using `malloc`) and initialize the TCBs with the function pointers, data parameter and status info. You can use the arrow operator (e.g. `task->`) to set these values.

    {: .note-title}
    > Aside:
    >
    > The entries in the `enum` of tasks were created in the order they are implemented into the TCB queue as it serves as an explicit way to address which task you are dealing with. (e.g.: `queue[1]->taskPtr = ...` vs `queue[TASK_GREEN]->taskPtr = ...`)

7. Before moving on to creating a task scheduler in the infinite loop, create a counter variable in `main()` starting at zero.
8. Timer interrupt handler. Now contains queue[TASK_CHOOSE]->taskReady = true;. This will ensure that the FSM logic will be called with every single tick of the RTOS.
9. Inside an empty `while(1)` loop now, we continuously cycle through the tasks (0-4) in order and execute the ones marked as ready, passing in the task's data pointer. E.g.: if(queue[i]->taskReady) {...}
10. It is important to note the taskReady parameter is not automatically set to false when being executed in our system. Keeping this value true will mean the function will execute every single complete cycle of the scheduler.

{: .info}
Some material covered in this lab was borrowed from Dr. Nelson's slides on real-time operating systems,
specifically the stripped-down implementation of a task control block. To read about the material in context, check
out [slides 38-40 here](http://csce.uark.edu/~ahnelson/CSCE4114/lectures/lecture14.pdf).

## Submission



## Prelab

None! Let's take it easy this week.. &#x1F642;

## References

AXI Timer Documentation
(<https://www.xilinx.com/support/documentation/ip_documentation/axi_timer/v2_0/pg079-axi-timer.pdf>)
AXI Interrupt Controller Documentation
(<https://www.xilinx.com/support/documentation/ip_documentation/axi_intc/v4_1/pg099-axi-intc.pdf>)
Dr. Nelson Chapter 14 Slides (<http://csce.uark.edu/~ahnelson/CSCE4114/lectures/lecture14.pdf>).
