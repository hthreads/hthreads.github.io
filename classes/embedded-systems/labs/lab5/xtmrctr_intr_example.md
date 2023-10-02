---
layout: page
title: Timer Interrupt Example
parent: Lab 5
grand_parent: Labs
nav_exclude: true
---

[< Back](./index.md#project-instructions){: .btn .btn-outline}

# Lab 5: Timer Interrupt Example

__Raw:__ [xtmrctr_intr_example.c](./assets/examples/xtmrctr_intr_example.c){: target="_blank"}

```c
// This file is a simplified example of timer interrupts adapted from
// xtmrctr_intr_example.c which can be found in the timer examples in the
// system.mss file in SDK
#include <xparameters.h>
#include <xtmrctr.h>
#include <xil_exception.h>
#include <xil_printf.h>
#include <xintc.h>

// The following constants map to the XPAR parameters created in the
// xparameters.h file. They are only defined here such that a user can easily
// change all the needed parameters in one place.
#define TMRCTR_DEVICE_ID XPAR_TMRCTR_0_DEVICE_ID
#define TMRCTR_INTERRUPT_ID XPAR_INTC_0_TMRCTR_0_VEC_ID
#define INTC_DEVICE_ID XPAR_INTC_0_DEVICE_ID

// The following constant determines which timer counter of the device that is
// used for this example, there are currently 2 timer counters in a device
// and this example uses the first one, 0, the timer numbers are 0 based
#define TIMER_0 0

// The following constant is used to set the reset value of the timer counter
#define LOAD_VALUE 41666667

// Function Prototypes
void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber);
void TmrCtrDisableIntr(XIntc *IntcInstancePtr, u16 IntrId);
void executionFailed();

// Variable declarations
XIntc InterruptController; // Create an instance of the interrupt controller
XTmrCtr TimerCounter;      // Create an instance of the Timer Counter

// The following variables are shared between non-interrupt processing and
// interrupt processing such that they must be global.
volatile int count = 0;

int main(void)
{
  // Setup structures and variables for execution loop: ---------------------
  volatile int *ledData = (int *)0x40000000;     // LED output register
  volatile int *ledTri = (int *)0x40000004;      // LED tristate register
  volatile int *rgbLedsData = (int *)0x40010000; // RGB output register
  volatile int *rgbLedsTri = (int *)0x40010004;  // RGB tristate register

  // Give variables initial conditions
  *ledTri = 0x0;
  *ledData = 0x0;
  *rgbLedsTri = 0x000;
  *rgbLedsData = 0x000;

  // For the following section, the functions return a status indication to
  // determine if the operation failed or not. This is stored in "status" and
  // will be reused for each API call.
  int status;

  // Initialize the timer counter instance
  status = XTmrCtr_Initialize(&TimerCounter, TMRCTR_DEVICE_ID);
  if (status != XST_SUCCESS)
  {
    xil_printf("Failed to initialize the timer! Execution stopped.\n");
    executionFailed();
  }

  // Verifies the specified timer is setup correctly in hardware/software
  status = XTmrCtr_SelfTest(&TimerCounter, 1);
  if (status != XST_SUCCESS)
  {
    xil_printf("Testing timer operation failed! Execution stopped.\n");
    executionFailed();
  }

  // Initialize the interrupt controller instance
  status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);
  if (status != XST_SUCCESS)
  {
    xil_printf("Failed to initialize the interrupt controller! Execution stopped.\n");
    executionFailed();
  }

  // Connect a timer handler that will be called when an interrupt occurs
  status = XIntc_Connect(&InterruptController,
                         TMRCTR_INTERRUPT_ID,
                         (XInterruptHandler)XTmrCtr_InterruptHandler,
                         (void *)&TimerCounter);
  if (status != XST_SUCCESS)
  {
    xil_printf("Failed to connect timer handler! Execution stopped.\n");
    executionFailed();
  }

  // Start the interrupt controller
  status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
  if (status != XST_SUCCESS)
  {
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
  XTmrCtr_SetHandler(&TimerCounter, TimerCounterHandler, &TimerCounter);

  // Set our timer options (setting TCSR register indirectly through Xil API)
  XTmrCtr_SetOptions(&TimerCounter, TIMER_0,
                     XTC_INT_MODE_OPTION | XTC_DOWN_COUNT_OPTION | XTC_AUTO_RELOAD_OPTION);

  // Set what value the timer should reset/init to (setting TLR indirectly)
  XTmrCtr_SetResetValue(&TimerCounter, TIMER_0, LOAD_VALUE);

  // Start the timer
  XTmrCtr_Start(&TimerCounter, TIMER_0);

  // Infinite Loop: ---------------------------------------------------------
  while (1)
  {
    *ledData = count;
  }
}

void executionFailed()
{
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
void TimerCounterHandler(void *CallBackRef, u8 TmrCtrNumber)
{
  XTmrCtr *InstancePtr = (XTmrCtr *)CallBackRef;

  /*
   * Check if the timer counter has expired, checking is not necessary
   * since that's the reason this function is executed, this just shows
   * how the callback reference can be used as a pointer to the instance
   * of the timer counter that expired, increment a shared variable so
   * the main thread of execution can see the timer expired
   */
  if (XTmrCtr_IsExpired(InstancePtr, TmrCtrNumber))
  {
    count++;
  }
}

// Optional demonstration on how to disable interrupt
void TmrCtrDisableIntr(XIntc *IntcInstancePtr, u16 IntrId)
{
  // Disable the interrupt for the timer counter
  XIntc_Disable(IntcInstancePtr, IntrId);

  return;
}
```
