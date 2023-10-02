---
layout: page
title: Lab 7
parent: Labs
grand_parent: EECS 4114 Embedded Systems
nav_exclude: true
---

# Lab 6: Autonomus Vehicle

## Week 1

### Introduction

This first week is devoted to assembling and testing your autonomous robot car. First you and your partner will modify your SoC to include the peripherals necessary for communication with the car sensors. Then you will follow the video instructions to assemble the shadow chassis, which is the base platform for your Arty Bot. Your TA help you through the assembly process. Zip ties and colorful "engineering" rubber bands will be able in the lab if you need them

### Lab Instructions

1. Go to [Digilent’s IP (Intellectual Property) repository](https://github.com/Digilent/vivado-library) on GitHub and download their repository of hardware sensor IP and interface definitions. Extract the downloaded zip file and remember the location on your computer where you extracted the files. Ideally, put the extracted folder next to your project folder.
2. Next, download this [GitHub repository](https://github.com/tkamucheka/CSCE4114-ip-repo) and extract it to a separate folder. This repository contains IP necessary for interfacing with the Pmod DHB1 and MAXSONAR hardware sensors.
3. Now let us make some changes to our SoC (System on Chip). Start by opening your project in Vivado. Navigate in the Flow Navigator on the left side to “Open Block Design.”
4. To add our downloaded IP to Vivado, navigate to “Project Settings” to open the project Settings dialog. In the left panel select “IP,” then select the “Repository Manager” tab. Now add the two IP repository folders you downloaded. Select “Apply” and close the dialog.
5. Back in the block design of your SoC. We want to add IP to the SoC for communicating with the external Pmod peripherals. We will add “AXI GPIO,” “Pmod_DHB1”, and “Pmod_Dual_MAXSONAR” IP for communication with the two line detecting light sensors, Pmod MAXSONAR sensor, and Pmod DHB1 motor driver
    1. AXI GPIO >> jA - Pmod LS1. Using the “Add IP” button and dialog. Add a new “AXI GPIO” and “Pmod Bridge” IP to your design. DO NOT USE connection automation. Connect the “S_AXI” bus port of the “AXI GPIO” to a slave port on the interconnect. Link to the two GPIO ports on the “AXI GPIO” with the “GPIO_Top_Row” and “GPIO_Bottom_Row” on the Pmod Bridge. Last, in the “Board” window on the left, right-click the “jA” port under Pmod and select “connect component.” In the dialog that appears, find the Pmod bridge you just added and select the “Pmod_out” port as your connection. One more thing, enable interrupts for the “AXI GPIO.”
    2. Pmod_Dual_MAXSONAR >> jB - Pmod MAXSONAR. Add the “Pmod_Dual_MAXSONAR” IP and connect it to a Pmod Bridge. You can connect the “Pmod_out” port of the Pmod Bridge to the Pmod port jB.
    3. Pmod_DHB1 >> jD – Pmod MAXSONAR. Add the “Pmod_DHB1_v1_0” IP and connect it to the jD Pmod port through a Pmod Bridge
6. Go to "Address Editor" window of the block design and and assign memory addresses to the newly added IP.
7. Validate your deign and generate a bitstream
8. Watch the assembly [instruction video](https://www.youtube.com/watch?v=aJRYTqZu5OE) or follow the [instructions given here](https://learn.sparkfun.com/tutorials/assembly-guide-for-redbot-with-shadow-chassis) to assembly the chassis of your robot.

### Project Assignment

1. Mount the two IR (infrared) Proximity sensors somewhere on the front of car. Try to mount them as close as possible to the floor. Connect the sensors to one of S1, S2, S3, or S4 pin headers on the Pmod LS1 module. Finally, connect the Pmod LS1 module to the Arty board via the Pmod port you connected the AXI GPIO in the board design.
2. Mount the two Pmod MAXSONAR sensors on the front of the car. Use the provided Pmod extension cables to connect the sensors to the Pmod port where you connected the AXI GPIO in the board design. Connect one sensor to the bottom row of the Pmod port and the other to the top row of the same Pmod port.
3. Connect the Pmod DHB1 to the Arty board via the Pmod port you selected in the board design. Now make the following connections on the Pmod DHB1 module:
    1. Connect the left and right motors to the screw terminals J5 and J6.
    2. Connect the Hall effect sensors to J7 and J8. The red wire is VCC, black is GND, and white is motor feedback. The motor feedback signal is used to count wheel revolutions using the Hall effect sensors.
    3. Connect the AA battery pack to the screw terminal at J4.

### Prelab Assignment

Take a look at the manuals listed in the references sections

1. [Tang, A. ArtyBot - A two-wheel robotics platform built on the Arty development board. Hackster.io](https://www.hackster.io/atang08/artybot-551478)
2. [Pmod Interface Specification](https://digilent.com/reference/_media/reference/pmod/pmod-interface-specification-1_3_1.pdf)
3. [Pmod LS1 Reference Manual](https://digilent.com/reference/pmod/pmodls1/reference-manual)
4. [Pmod MAXSONAR Reference Manual](https://digilent.com/reference/pmod/pmodmaxsonar/reference-manual)
5. [Pmod DHB1 Reference Manual](https://digilent.com/reference/pmod/pmoddhb1/reference-manual)

---

## Week 2

When determining which scheduling solution your embedded system should use, there are a variety of options that range from a simplistic bare-metal design, like we used in lab 6, to a full embedded Linux distribution (called PetaLinux). Choosing one may come down to factors like:

* **Responsiveness and compatibility:** Companies like TI or Xilinx may create their own RTOS (e.g.: TI-RTOS or XilKernel) that work the best on the hardware they tailor it for. Some also follow POSIX standards such that maximum compatibility exists for programming between different devices (e.g.: You can create pthreads on your computer, as well as TI-RTOS and XilKernel as they are POSIX compliant, but not the standard FreeRTOS).
* **System resources:** Larger embedded systems may have multi-core systems that support embedded Linux or a combination of systems (PetaLinux + FreeRTOS).
* **Licensing:** Certain software packages like Azure RTOS and others commercially available for cloud IoT uses require licensing.

For this lab, we will be using FreeRTOS to convert our bare-metal task switcher into a full RTOS-driven system that supports preemption and has a programming feel that is closer to that of programming multi-threaded programs for computers. FreeRTOS has been described as "the de facto standard" and "market leading" because of its robustness, the wide range of microprocessors supported, and lack of a license needed to use it. Xilinx has deprecated the use of their "XilKernel" system and have since removed it in versions of SDK > 2016. FreeRTOS provides a small, robust, and modular kernel. It is highly integrated with the Xilinx SDK and is a free software library option that can be selected when creating a new project. It allows a very high degree of customization, letting users tailor the kernel to an optimal level both in terms of size and functionality with most modifications readily available in the "FreeRTOS.h" and "FreeRTOSConfig.h" header files. It supports the core features required in a lightweight embedded kernel, however, the version included with the Xilinx 2016.4 SDK is not POSIX compliant and does not support pthreads. FreeRTOS can be used to implement higher level services (such as networking, video, and audio) and subsequently run applications using these services.

For this lab, we will be using the same SDK workspace folder provided for labs 5 and 6 to create the FreeRTOS project and migrate our traffic signal from the bare-metal design. If you need to download a fresh copy of the workspace, it can be found here.

### Project Directions

* Start off by downloading [this repository](https://github.com/tkamucheka/embedded-systems-lab), which has a few examples for working with your car.
* Open your workspace from previous labs and go to File > New > Application Project. If for some reason you do not have a workspace, use the examples workspace in the cloned repo.
* When the "New Project" preferences window appears, name the project "lab7" and in the "OS Platform" selection, choose freertos823_xilinx. (Note: if you are using a different version of SDK, your version of FreeRTOS may vary). Click next and choose the empty project option, then click finish.
* Copy the files (artyBotLib.h, artyBotLib.c, motorControl.h, motorControl.c, pidController.h, pidController.c) from the ArtyBot_example0/src example directory and paste them into your newly created project's src directory.
* With the newly created project, right click on the src folder and create a new file (New > File). Name the file "main.c" and paste the following contents in it as a starting point:

```C
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xintc.h"
/* Other includes */
#include "artyBotLib.h"
#include "PmodMAXSONAR.h"

// Car states
typedef enum {IDLE, DRIVE, NAVIGATE} State;

State state = IDLE; // Shared resource. Mutex driven.

SemaphoreHandle_t state_mutex;

/* FreeRTOS Tasks */
// ... 

/* Main Function */
int main(void)
{
  // Setup and init code
  // Start FreeRTOS Kernel
  return 0;
}
```

* We will attempt to develop our car with a structure similar to the traffic signal from previous labs. Let's attempt to first migrate part of the "task chooser" thread that was used in in the previous lab to govern when the other tasks were supposed to run.
  * To create a thread in FreeRTOS, we first need to create a task handle struct that the kernel will use to associate a task with. Above main(), create a handle called `SupervisorTaskHandle` and initialize it to null, like so: `TaskHandle_t SupervisorTaskHandle = NULL`;
  * Next, we need to create a task handler function that will execute. Similar to last lab, create a function that will act as a thread runner that takes in a void pointer parameter, like: `void SupervisorTask(void *arg) {...}`. Fill out the contents of this function with just an infinite loop.
  * Now we are going to formally create the task with xCreateTask. That macro has the following parameters: `xCreateTask(pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask)`. These parameters are explained by hovering over the macro name, right-clicking, and opening the declaration (or F3). I've also provided the parameter details below:
    * **pvTaskCode** Pointer to the task entry function. Tasks must be implemented to never return (i.e. continuous loop).
    * **pcName** A descriptive name for the task. This is mainly used to facilitate debugging. Max length defined by configMAX_TASK_NAME_LEN - default is 16. e.g.: "Supervisor"
    * **usStackDepth** The size of the task stack specified as the number of variables the stack can hold - not the number of bytes. For example, if the stack is 16 bits wide and usStackDepth is defined as 100, 200 bytes will be allocated for stack storage.
    * **pvParameters** Pointer that will be used as the parameter for the task being created.
    * **uxPriority** The priority at which the task should run. Systems that include MPU support can optionally create tasks in a privileged (system) mode by setting bit portPRIVILEGE_BIT of the priority parameter. For example, to create a privileged task at priority 2 the uxPriority parameter should be set to `( 2 | portPRIVILEGE_BIT )`.
    * **pvCreatedTask** Used to pass back a handle by which the created task can be referenced
  * So our xTaskCreate will look like: `xTaskCreate(SupervisorTask, "Supervisor", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &SupervisorTaskHandle);` Notice how we are just passing NULL for the parameter and that we are passing the address of the TaskHandle type. If you pass the value directly, your tasks won't be able to suspend/resume if you attempt to use the functions.
  * Now we must acknowledge an issue before moving forward. We plan on having several tasks being able to write to the state variable when that thread has deemed the condition should move to the next state. E.g.: the IDLE task will set the state to DRIVE, the STOP task will set the state to IDLE and so on. The issue arises when we have multiple threads with the ability to write a variable and a thread trying to read the variable in our preemptive system. We are going to need to implement a mutex around our state variable to ensure that it is written to by one thread at a time.
    * Go back up to the top of the file where the state variable is globally defined and add the following line below it: `SemaphoreHandle_t state_mutex;` Then back in the main function, we can setup the mutex with the line: `state_mutex = xSemaphoreCreateMutex();`
    * With this mutex, we can now try and acquire the lock with `xSemaphoreTake(state_mutex, TRY_TICKS)` and we can unlock with `xSemaphoreGive(state_mutex)`. Notice TRY_TICKS in the semaphore take function. It represents the number of RTOS ticks (time unit, by default set to 100 ticks/sec) that the system will try to acquire the lock within, if it can't within the number provided, it will just fail.
    * Now in our SupervisorTask function infinite loop, we can see if the mutex is locked or unlocked using the return value of a function in an if statement:
    * Now we can prime which thread needs to run in our Supervisor Task similar to how we did in in the previous lab with the ready flag being set to true/false. `vTaskSuspend(/* TaskHandle_t type */)` will suspend a task from the scheduler and `vTaskResume(/* TaskHandle_t type */)` will add a task back to the pool of tasks to execute. You can add these function calls to the switch statement cases above to set which thread will execute after running the supervisor.
    * At the end of this block of code when we can make the thread go to sleep for a set amount of time. `vTaskDelay(/* num of ticks */)` will let the task sleep for a certain number of ticks. Notice this is not seconds, as 1 tick = 1/100 second. There is a simple macro to convert ticks to seconds and can be used as a parameter in the vTaskDelay function to sleep for a certain number of milliseconds: `vTaskDelay( pdMS_TO_TICKS(/* ms to sleep */) )`.
    * > Side note: if you are trying to make an LED flash or stay lit for a certain period of time, the vTaskDelay call in that particular task function should make that task trivial.

```C
while(1)
{
  // Acquire mutex lock
  if( xSemaphoreTake(state_mutex, 10) )
  {
    // We have access to the mutex. 
    // Do critical operations here

    switch(state) {/* ... */}
    
    // Release lock
    xSemaphoreGive(state_mutex);
  }

  // Sleep for a small amount of time
  // ...
}
```

* To start the kernel, add the line `vTaskStartScheduler();` right before the return statement in the main function.
* Now that we have gone through the basis of recreating the task choose thread from the previous lab, go ahead a create some tasks that correspond to driving your car. You should have all the function calls you need.
* The example project ArtyBot_example0 has some examples of what your states can look like. That being sad, there are many different ways to get this project done. Add extra states if you need them.

### Project Assignment

* Turn your car into a line following robot. The line will be a (white) duct tape track and your car should drive around the track with wheels on either side of the track. There will be an obstacle at the end of the track, use this to stop your car. To do this, you will need:
  * Use the GPIO PMOD LS1 to detect when your car drives over the line and steer that car back onto the track
  * Use the PMOD MAXSONAR to detect obstacles and stop the car before the obstacle.
  * Program the Arty's non-volatile 16 MB Quad-SPI flash memory with your SoC and application so that your application runs when you power on the Arty board. NB: the file names in this section may be different from what you have in your project. Just remember what you named your soc in Vivado and project name in the SDK and the rest should look familiar. If you're unsure ask your TA for help. To program your Arty
    * Right click your project folder, the project you want to flash onto the board, and select **Generate Linker Script**. On the right side, change all 3 dropdowns from *microblaze_0_local*... to *mig_7series_0_memaddr*. If you are on Linux this might not work. You may need to open the **lscript.ld** file in your project's src directory and manually change all the entries to *mig_7series_0_memaddr*.
    * Click the **Program Flash** button in the toolbar. Next to Image File, click Browse and navigate to your project folder, then into Debug. Select your_project_name.elf and click open. Next to Offset, enter `0x00C00000`. This is the address offset where your program will be stored in the flash memory. Next to 'Flash Type,' select *S25FL128sxxxxxx0-spi-x1_x2_x4*. **IMPORTANT**, make sure 'Convert ELF to bootloadable SREC format and program' is checked. Click **Program**.
    * To create a bootloader, in the toolbar select File > New > Application Project. Name the project something like 'bootloader' as shown in the image above. Click **Next**.
    * Select **SREC SPI Bootloader**, and select **Finish**.
    * We need to make a few changes to our bootloader before loading it onto the Arty board. First, we need to set the 'address offset' so the bootloader can find our application. In the 'Bootloader' project, open the blconfig.h file and change the **FLASH_IMAGE_BASEADDR** to `0x00C00000`. Note, this same address you used when you flashed your application onto the flash memory earlier.
    * > **Optional**, if you wish to improve your ArtyBot's startup time. Edit the file bootloader.c and comment out the line `#define VERBOSE`. This will turn off console logging. **NB:** Console logs may be useful when you're initially setting things up.
    * Click the **Program FPGA** button in the toolbar. In the right half of the Software Configuration section, there is a cell that says `bootloop`. Click the right side of this cell to reveal a drop-down menu. Select **Bootloop.elf** from the list, then click Program. This will generate a bit file with the MicroBlaze's memory initialized with the program that you have written. The Arty is also programmed and if everything went well you should see your application running in your favorite console application (e.g. SDK Terminal, Putty or TeraTerm)
    * Almost there, click the **Program Flash** button in the toolbar. Next to Image File, click Browse and navigate to the SDK workspace, then into `base_soc_wrapper_hw_platform_0`. Select download.bit and click **Open**. Next of Offset, enter `0x0`. Next to Flash Type, select `S25FL128sxxxxxx0-spi-x1_x2_x4`. Click **Program**.
    * Congratulations, you have flashed the bootloader and your application to the non-volatile SPI Qflash memory. Next time you power on the board, the bootloader will be copied from the flash memory and when it runs, it will execute your application.

### Prelab Assignment

None this week. Lucky you!

### References

1. FreeRTOS Manual [https://www.freertos.org/FreeRTOS-quick-start-guide.html](https://www.freertos.org/FreeRTOS-quick-start-guide.html) (expand API Reference on the side for full documentation on function calls)
