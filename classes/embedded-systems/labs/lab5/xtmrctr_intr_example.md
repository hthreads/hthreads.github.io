---
layout: page
title: Timer Interrupt Example
nav_exclude: true
---

[< Back](./index.md#project-instructions){: .btn .btn-green}

# Lab 5: Timer Interrupt Example

__Raw:__ [xtmrctr_intr_example.c](./assets/examples/xtmrctr_intr_example.c){: target="_blank"}

```c
/******************************************************************************
* Copyright (C) 2002 - 2021 Xilinx, Inc.  All rights reserved.
* Copyright (C) 2022 - 2024 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/******************************************************************************
 *
 * @file xtmrctr_intr_example.c
 *
 * This file is a simplified example of timer interrupts adapted from
 * xtmrctr_intr_example.c which can be found in the timer examples within Vitis
 * examples.
 *
 * This example shows the use of the Interrupt Controller both with a PowerPC
 * and MicroBlaze processor.
 *
 * Revision:
 * @author: Tendayi Kamucheka (ftendayi@gmail.com)
 * @date: 09/26/2024
 ******************************************************************************/

#include "xil_exception.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "xintc.h"
#include "xparameters.h"
#include "xtmrctr.h"

// Device IDs:
// Only one interrupt controller and timer counter are used in the hardware 
// design. So the device IDs are set to 0 for both.
#define INTC_DEVICE_ID   0
#define TMRCTR_DEVICE_ID 0

/*
 *  This is the Interrupt Number of the Device whose Interrupt Output is
 *  connected to the Input of the Interrupt Controller.
 *  - XPAR_FABRIC_XTMRCTR_0_INTR is the interrupt ID 
 *    of the AXI Timer IP that is connected to the interrupt 
 *    controller. Definition in xparameters.h.
 *  - These ids are generated based the order you connected your 
 *    interrupt sources to the Concat IP.
 *    i.e.:
 * 
 *    (0) axi_timer_0 ---\
 *                        ---> Concat ---> axi_intc_0
 *    (1) axi_gpio_0  ---/
 */
#define TMRCTR_INTERRUPT_ID XPAR_FABRIC_XTMRCTR_0_INTR

// The following constant determines which timer counter of the device that is
// used for this example, there are currently 2 timer counters in a device
// and this example uses the first one, 0, the timer numbers are 0 based
// NB: XTC_TIMER_0 is defined in xtmrctr.h.
#define TIMER_0 XTC_TIMER_0

// The following constant is used to set the reset value of the timer counter
// to half the CPU frequency. This value is loaded into the timer counter such
// that the timer counter will expire after half a second.
// i.e. CPU_Freq (Hz) / 2
#define LOAD_VALUE 81247969 >> 1

// Function Prototypes:
void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber);
void TmrCtrDisableIntr(XIntc *IntcInstancePtr, u16 IntrId);
void executionFailed();

// Object declarations:
XIntc InterruptController; // Create an instance of the interrupt controller
XTmrCtr TimerCounter;      // Create an instance of the Timer Counter

// The following variables are shared between non-interrupt processing and
// interrupt processing such that they must be global.
volatile int count = 0;

// GPIO Registers:
volatile int *ledData;     // LED output register
volatile int *ledTri;      // LED tristate register
volatile int *rgbLedsData; // RGB output register
volatile int *rgbLedsTri;  // RGB tristate register

int main(void) {
  // Setup structures and variables for execution loop: ---------------------
  ledData     = (int *)(0x40000000); // LED output register
  ledTri      = (int *)(0x40000004); // LED tristate register
  rgbLedsData = (int *)(0x40010000); // RGB output register
  rgbLedsTri  = (int *)(0x40010004); // RGB tristate register

  // Give variables initial conditions
  *ledTri      = 0x0;
  *ledData     = 0x0;
  *rgbLedsTri  = 0x000;
  *rgbLedsData = 0x000;

  // For the following section, the functions return a status indication to
  // determine if the operation failed or not. This is stored in "status" and
  // will be reused for each API call.
  int status;

  // Initialize the timer counter instance
  status = XTmrCtr_Initialize(&TimerCounter, TMRCTR_DEVICE_ID);
  if (status != XST_SUCCESS) {
    xil_printf("Failed to initialize the timer! Execution stopped.\n");
    executionFailed();
  }

  // Verifies the specified timer is setup correctly in hardware/software
  status = XTmrCtr_SelfTest(&TimerCounter, 1);
  if (status != XST_SUCCESS) {
    xil_printf("Testing timer operation failed! Execution stopped.\n");
    executionFailed();
  }

  // Initialize the interrupt controller instance
  status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);
  if (status != XST_SUCCESS) {
    xil_printf(
        "Failed to initialize the interrupt controller! Execution stopped.\n");
    executionFailed();
  }

  // Connect the timer's interrupt handler function `XTmrCtr_InterruptHandler`
  // that will be called when an interrupt occurs.
  // NB: We cast the interrupt handler function to type `XInterruptHandler`.
  status = XIntc_Connect(&InterruptController, TMRCTR_INTERRUPT_ID,
                         (XInterruptHandler)XTmrCtr_InterruptHandler,
                         (void *)&TimerCounter);
  if (status != XST_SUCCESS) {
    xil_printf("Failed to connect timer handler! Execution stopped.\n");
    executionFailed();
  }

  // Start the interrupt controller
  status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
  if (status != XST_SUCCESS) {
    xil_printf("Failed to start interrupt controller! Execution stopped.\n");
    executionFailed();
  }

  // Enable interrupts and the exception table
  XIntc_Enable(&InterruptController, TMRCTR_INTERRUPT_ID);
  Xil_ExceptionInit();

  // Register the interrupt controller handler with the exception table.
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                               (Xil_ExceptionHandler)XIntc_InterruptHandler,
                               &InterruptController);

  Xil_ExceptionEnable();

  // Set the handler (function pointer) that we want to execute when the
  // interrupt occurs
  // NB: We cast the handler function to type `XTmrCtr_Handler`.
  XTmrCtr_SetHandler(&TimerCounter, (XTmrCtr_Handler)TimerCounterHandler,
                     &TimerCounter);

  // Set our timer options (setting TCSR0 register indirectly through Xil API)
  // We can set multiple options by using the bitwise OR operator. The options
  // used here are defined in xtmrctr.h.
  u32 options =
      XTC_INT_MODE_OPTION | XTC_DOWN_COUNT_OPTION | XTC_AUTO_RELOAD_OPTION;
  XTmrCtr_SetOptions(&TimerCounter, TIMER_0, options);

  // Set what value the timer should reset/init to (setting TLR0 indirectly)
  XTmrCtr_SetResetValue(&TimerCounter, TIMER_0, LOAD_VALUE);

  // Start the timer
  XTmrCtr_Start(&TimerCounter, TIMER_0);

  // Infinite Loop: ---------------------------------------------------------
  while (1) {
    *ledData = count;
  }
}

void executionFailed() {
  *rgbLedsData = 04444; // display all red LEDs if fail state occurs

  // trap the program in an infinite loop
  while (1)
    ;
}

/*****************************************************************************/
/**
 * This function is the handler which performs processing for the timer counter.
 * It is called from an interrupt context such that the amount of processing
 * performed should be minimized.  It is called when the timer counter expires
 * if interrupts are enabled.
 *
 * This handler provides an example of how to handle timer counter interrupts
 * but is application specific.
 *
 * @param CallBackRef is a pointer to the callback function
 * @param TmrCtrNumber is the number of the timer to which this
 *  handler is associated with.
 *
 * @return None.
 *
 * @note  None.
 *
 ******************************************************************************/
void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber) {
  XTmrCtr *InstancePtr = (XTmrCtr *)CallBackRef;

  /*
   * Check if the timer counter has expired, checking is not necessary
   * since that's the reason this function is executed, this just shows
   * how the callback reference can be used as a pointer to the instance
   * of the timer counter that expired, increment a shared variable so
   * the main thread of execution can see the timer expired.
   * - In the event that the timer counter is not expired but somehow an
   *   interrupt is generated, the count will not be incremented. Think cosmic
   *   rays or other sources of noise.
   */
  if (XTmrCtr_IsExpired(InstancePtr, TmrCtrNumber)) {
    count++;
  }
}

// Optional demonstration on how to disable interrupt
void TmrCtrDisableIntr(XIntc *IntcInstancePtr, u16 IntrId) {
  // Disable the interrupt for the timer counter
  XIntc_Disable(IntcInstancePtr, IntrId);

  return;
}

```
