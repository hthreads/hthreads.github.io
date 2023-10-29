---
layout: page
title: Lab 7 
parent: Labs
grand_parent: EECS 4114 Embedded Systems
nav_exclude: true
---

# Lab 7: FreeRTOS

## Introduction

When determining which scheduling solution your embedded system should use, there are a variety of options that range from a simplistic bare-metal design, like we used in lab 6, to a full embedded Linux distribution (called PetaLinux). Choosing one may come down to factors like:

- **Responsiveness and compatibility:** Companies like TI and Xilinx may create their own RTOS (e.g. TI-RTOS or XilKernel *[[discontinued...](https://support.xilinx.com/s/article/69214?language=en_US)]*) that work best on the hardware they design. Some also follow POSIX standards such that maximum compatibility exists for programming between different devices (e.g.: You can create pthreads on your computer, as well as TI-RTOS and XilKernel as they are POSIX compliant, but not the standard FreeRTOS).
- **System resources:** Larger embedded systems may have multi-core systems that support embedded Linux or a combination of systems (PetaLinux + FreeRTOS).
- **Licensing:** Certain software packages like Azure RTOS and others commercially available for cloud IoT uses require licensing.

For this lab, we will be using [FreeRTOS](https://www.freertos.org) to convert our bare-metal task switcher into a full RTOS-driven system that supports task preemption and has a programming feel that is closer to that of programming multi-threaded programs for computers. FreeRTOS has been described as "the de facto standard" and "market leading" because of its robustness, the wide range of microprocessors supported, and lack of a license needed to use it. Xilinx has deprecated the use of their "XilKernel" system and have since removed it in SDK version 2017.1 and onwards. FreeRTOS provides a small, robust, and modular kernel. It is highly integrated with the Xilinx SDK and is a free software library option that can be selected when creating a new project. It allows a very high degree of customization, letting users tailor the kernel to an optimal level both in terms of size and functionality with most modifications readily available in the "FreeRTOS.h" and "FreeRTOSConfig.h" header files. It supports the core features required in a lightweight embedded kernel, however, the version included with the Xilinx SDK is not POSIX compliant and does not support pthreads. FreeRTOS can be used to implement higher level services (such as networking, video, and audio) and subsequently run applications using these services.

{: .note}
> We will be using the same SDK workspace from previous labs to create the FreeRTOS project and migrate our traffic signal from the bare-metal design. 

## Project Directions

1. Open your workspace from previous labs and go to `File > New > Application Project`. 
2. In the **"New Project"** preferences window, name the project (e.g. "lab7") and for the **"OS Platform"** selection, choose `freertos10_xilinx`. Click **Next** and choose the empty project option, then click **Finish**.   
    > _If you are using a different version of the SDK, your version of FreeRTOS may vary._
3. In the newly created project, right-click on the `src` folder and create a new source file `New > Source File`. Name the file "main.c" and paste the following code in it as a starting point:  
    
    ```c
    /* FreeRTOS includes. */
    #include "FreeRTOS.h"
    #include "task.h"
    #include "semphr.h"
    /* Xilinx includes. */
    #include "xil_printf.h"
    #include "xparameters.h"
    #include "xgpio.h"
    #include "xintc.h"
    
    // Traffic light states
    typedef enum
    {
        GREEN,
        RED,
        RED_BLINK_START,
        RED_BLINK_END
    } State;
    
    // Default state
    State state = GREEN;
    
    // Shared resource. Mutex driven.
    SemaphoreHandle_t state_mutex;
    
    /* FreeRTOS Tasks */
    
    /* Main Function */
    int main(void)
    {
        // Setup and init code
    
        // Start FreeRTOS Kernel
    
        return 0;
    }
    ```

    {: .info }
    > We will attempt to develop our traffic light with a structure similar to the traffic signal from previous labs. Let's attempt to first migrate part of the `taskChoose` thread that was used in the previous lab to govern when the other tasks were supposed to run.

4. To create a thread in FreeRTOS, we first need to create a task handle struct that the kernel will use to associate a task with. Above `main()`, create a handle called SupervisorTaskHandle and initialize it to null: `TaskHandle_t SupervisorTaskHandle = NULL;`
5. Next, we need to create a task handler function that will be executed for the supervisor task. Similar to last lab, create a function that will act as a thread runner that takes in a void pointer parameter: `void SupervisorTask(void *arg){...}`. Fill out the body of this function with just an infinite loop.
6. Now we are going to formally create the supervisor task with `xCreateTask`. This macro has the following parameters: `xCreateTask(pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask)`. These parameters are explained by hovering over the macro name, right-clicking, and opening the declaration (or F3). The parameter details are also provided below:
    - **pvTaskCode** Pointer to the task entry function. Tasks must be implemented to never return (i.e. continuous loop).
    - **pcName** A descriptive name for the task. This is mainly used to facilitate debugging. The max length is defined by `configMAX_TASK_NAME_LEN` - default is 10. (e.g. "Supervisor")
    - **usStackDepth** The size of the task stack specified as the number of variables the stack can hold - not the number of bytes. For example, if the stack is 16 bits wide and `usStackDepth` is defined as 100, 200 bytes will be allocated for stack storage.
    - **pvParameters** Pointer that will be used as the parameter for the task being created.
    - **uxPriority** The priority at which the task should run. Systems that include MPU support can optionally create tasks in a privileged (system) mode by setting bit `portPRIVILEGE_BIT` of the priority parameter. For example, to create a privileged task at priority 2 the `uxPriority` parameter should be set to `( 2 | portPRIVILEGE_BIT )`.
    - **pvCreatedTask** Used to pass back a handle by which the created task can be referenced.
    - So, in the `main` function, our call to `xTaskCreate` will look like:  
    `xTaskCreate(SupervisorTask, "Supervisor", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &SupervisorTaskHandle);`.  
    
    {: .warning }
    > Notice how we are just **passing NULL for the parameter** and that we are **passing the address of the TaskHandle**. If you pass the value directly, your tasks won't be able to suspend/resume when you attempt to use the functions.
    

7. Now we must acknowledge an issue before moving forward. We plan on having several tasks being able to write to the state variable when that thread has deemed the condition should move to the next state (e.g. the `GreenTask` task will set the state to `RED_BLINK_START`, and the `RedBlinkStartTask` task will set the state to `RED` and so on.) The issue arises when we have multiple threads with the ability to write to a variable and another thread (e.g. `SupervisorTask`) trying to read the variable in our preemptive system. We are going to need to implement a mutex around our state variable to ensure that it is written to by one thread at a time.
    1. Go back up to the top of the file where the state variable is globally defined and add the following line below it: `SemaphoreHandle_t state_mutex;` Then back in the `main()` function, we can setup the mutex with the line: `state_mutex = xSemaphoreCreateMutex();`
    2. With this mutex, we can now try and acquire a lock with `xSemaphoreTake(state_mutex, TRY_TICKS)` and we can unlock with `xSemaphoreGive(state_mutex)`. Notice `TRY_TICKS` in the semaphore take function. It represents the number of RTOS ticks _(time unit, by default set to 100 ticks/sec)_ that the system will try to acquire the lock within, if it cannot within the number provided, it will just fail.
    3. Now in our `SupervisorTask` function infinite loop, we can see if the mutex is locked or unlocked using the return value of the function `xSemaphoreTake()` in an if statement:    
    
        ```c
        while(1)
        {
            if (xSemaphoreTake(state_mutex, 10))
            {
                // We have acces to the mutex. Do critical operations here
                switch(state) { /* put FSM logic here */ }
                
                // Release mutex
                xSemaphoreGive(state_mutex);
            }
        }
        ```    
    
    4. Now we can prime which thread needs to run in our Supervisor task similar to how we did in the previous lab with the ready flag being set to true/false.  
    `vTaskSuspend(/* TaskHandle_t type */)` will suspend a task from the scheduler and `vTaskResume(/* TaskHandle_t type */)` will add a task back to the pool of tasks to execute. You can add these function calls to the `switch` statement cases to set which thread will execute after running the supervisor.
    5. At the end of the block of code in each task, we can make the thread go to sleep for a set amount of time. `vTaskDelay(/* num of ticks */)` will let the task sleep for a certain number of ticks. Notice this is not seconds, 1 tick = 1/100 seconds. There is a simple macro to convert ticks to seconds that can be used as a parameter in the `vTaskDelay` function to sleep for a certain number of milliseconds: `vTaskDelay( pdMS_TO_TICKS(/* ms to sleep */) )`.
    
    {: .highlight }
    > **Side note,** if you are trying to turn the LEDs on/off for a certain amout of time, calling `vTaskDelay()` in that particular task function should make that a trivial task.

8. To start the kernel, add the line `vTaskStartScheduler();` right before the `return` statement in the `main` function.
9. Now that we have gone through the basis of recreating the `taskChoose` thread from the previous lab, go ahead a create some tasks that correspond to the other states of the traffice light. You should have all the function calls you need. An example task is given below:  

    ```c
    void greenTask(void *arg) {
        unsigned int button_flag = 0;

        while(1) {
            /* 
            task logic:
            if (button is pressed)
                button flag = 1

            if (take mutex)
                state = RED_BLINK_START
                button flag = 0;
                release mutex
            */

            /* suspend ourselves */
            vTaskSuspend(NULL);
        }
    }
    ```

## Submission

1. Complete a lab report detailing how you completed this week lab. In your lab report compare and contrast the differences between your design and [Lab6: Bare Metal TCB](https://hthreads.github.io/classes/embedded-systems/labs/lab6)
2. Submit lab report and code file separately on Blackboard.

## Prelab

- Read the [FreeRTOS Manual](https://www.freertos.org/FreeRTOS-quick-start-guide.html). Expand the section **API Reference** on the left-side menu for full documentation on FreeRTOS function calls.

## References

1. [FreeRTOS Manual](https://www.freertos.org/FreeRTOS-quick-start-guide.html)

