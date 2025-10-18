---
layout: page
title: Simulated Interrupt Example
nav_exclude: true
---

[< Back](./index.md#lab-directions){: .btn .btn-green}

__Raw:__ [tasks.c](./assets/examples/tasks.c){: target="_blank"}

```c
/*
 * tasks.c
 *
 *  Bare Metal (no OS) example of a simple task scheduler
 *
 */

#include <stdlib.h>
#include <xgpio.h>
#include <xgpio_l.h>
#include <xil_exception.h>
#include <xil_printf.h>
#include <xintc.h>
#include <xparameters.h>
#include <xtmrctr.h>
#include <xtmrctr_l.h>

//===---------------------------------------------------------------------===//
// Constants
//===---------------------------------------------------------------------===//

// 5-ms period
#define LOAD_VALUE 406239

// RGB color definitions
#define RGB_RED 04444
#define RGB_GREEN 02222
#define RGB_OFF 0

// GPIO channel definitions
#define XGPIO_CHANNEL_1 1
#define XGPIO_CHANNEL_2 2

//===---------------------------------------------------------------------===//
// Function Prototypes
//===---------------------------------------------------------------------===//

// Tasks
void taskChoose(void *data);
void taskGreen(void *data);
void taskRedBlinkStart(void *data);
void taskRed(void *data);
void taskRedBlinkEnd(void *data);

// Interrupt Service Routines
void timer_ISR(void *CallBackRef, u8 TmrCtrNumber);
void button_ISR(void *CallBackRef);

// Helper functions
int platform_init();
void TmrCtrDisableIntr(XIntc *IntcInstancePtr, u16 IntrId);
void executionFailed();

//==----------------------------------------------------------------------===//
// Type Definitions
//==----------------------------------------------------------------------===//

// Enum type for the states
typedef enum { GREEN, FLASH_RED_START, RED, FLASH_RED_END } state_t;

// Enum type for the tasks
typedef enum {
  TASK_CHOOSE,
  TASK_GREEN,
  TASK_RED_BLINK_START,
  TASK_RED,
  TASK_RED_BLINK_END,
  MAX_TASKS
} task_t;

// Task Control Block (TCB) structure
typedef struct {
  void (*taskPtr)(void *);
  void *taskDataPtr;
  u8 taskReady;
} TCB_t;

//==----------------------------------------------------------------------===//
// Globals
//==----------------------------------------------------------------------===//

// Hardware instances
XIntc InterruptController; // Instance of the Interrupt Controller
XTmrCtr Timer;             // Instance of the Timer
XGpio btnGpio;             // Instance of the AXI_GPIO_0
XGpio ledGpio;             // Instance of the AXI_GPIO_1

// Task queue
TCB_t *queue[MAX_TASKS];

// Global variables
state_t state, nextState; // State variables
u8 buttonFlag = FALSE;    // Flag to indicate if the button is pressed
u32 systemTime = 0;       // System time: 1 unit = 5 ms
u32 startTime = 0;        // Start time of the current state
u32 elapsedTime = 0;      // Elapsed time since last state transition
u32 ledState = 0;         // Current state of the LEDs

//===---------------------------------------------------------------------===//
// Main
//===---------------------------------------------------------------------===//

int main(void) {
  // Initialize the platform:
  // Setup the GPIO, Interrupt Controller and Timer
  int status = XST_FAILURE;
  status = platform_init();
  if (status != XST_SUCCESS) {
    xil_printf("Failed to initialize the platform! Execution stopped.\n");
    executionFailed();
  }

  // Initialize the task queue and TCBs
  // Task 0: taskChoose
  queue[TASK_CHOOSE] = malloc(sizeof(TCB_t));
  queue[TASK_CHOOSE]->taskPtr = taskChoose;
  queue[TASK_CHOOSE]->taskDataPtr = NULL;
  queue[TASK_CHOOSE]->taskReady = FALSE;
  // Task 1: taskGreen
  queue[TASK_GREEN] = malloc(sizeof(TCB_t));
  queue[TASK_GREEN]->taskPtr = taskGreen;
  queue[TASK_GREEN]->taskDataPtr = NULL;
  queue[TASK_GREEN]->taskReady = FALSE;
  // Task 2: taskRedBlinkStart
  queue[TASK_RED_BLINK_START] = malloc(sizeof(TCB_t));
  queue[TASK_RED_BLINK_START]->taskPtr = taskRedBlinkStart;
  queue[TASK_RED_BLINK_START]->taskDataPtr = NULL;
  queue[TASK_RED_BLINK_START]->taskReady = FALSE;
  // Task 3: taskRed
  queue[TASK_RED] = malloc(sizeof(TCB_t));
  queue[TASK_RED]->taskPtr = taskRed;
  queue[TASK_RED]->taskDataPtr = NULL;
  queue[TASK_RED]->taskReady = FALSE;
  // Task 4: taskRedBlinkEnd
  queue[TASK_RED_BLINK_END] = malloc(sizeof(TCB_t));
  queue[TASK_RED_BLINK_END]->taskPtr = taskRedBlinkEnd;
  queue[TASK_RED_BLINK_END]->taskDataPtr = NULL;
  queue[TASK_RED_BLINK_END]->taskReady = FALSE;

  // Initialize the state machine
  state = nextState = GREEN;

  // Main loop
  while (1) {
    // Iterate through task queue and
    // execute tasks that are ready
    for (int i = 0; i < MAX_TASKS; i++) {
      if (queue[i]->taskReady) {
        // Execute current ready the task
        (*(queue[i]->taskPtr))(queue[i]->taskDataPtr);
      }
    }

    // Update the LEDs
    XGpio_DiscreteWrite(&ledGpio, XGPIO_CHANNEL_1, ledState);
  }

  return 0;
}

//===---------------------------------------------------------------------===//
// Task Definitions
//===---------------------------------------------------------------------===//

void taskChoose(void *data) {
  // DEBUG: Print the current state
  // xil_printf("Choose task...\n");

  // Update state
  state = nextState;

  // Wake current task
  switch (state) {
  case GREEN:
    queue[TASK_GREEN]->taskReady = TRUE;
    break;
  case FLASH_RED_START:
    queue[TASK_RED_BLINK_START]->taskReady = TRUE;
    break;
  case RED:
    queue[TASK_RED]->taskReady = TRUE;
    break;
  case FLASH_RED_END:
    queue[TASK_RED_BLINK_END]->taskReady = TRUE;
    break;
  }

  // Task puts itself to sleep
  queue[TASK_CHOOSE]->taskReady = FALSE;
}

void taskGreen(void *data) {
  // DEBUG: Print the current state
  // xil_printf("Green task...\r\n");

  // Set the LEDs to green
  ledState = RGB_GREEN;

  // Set the next state
  if (buttonFlag) {
    nextState = FLASH_RED_START;
    startTime = systemTime; // Reset the start time
    ledState = RGB_OFF;     // Set the LEDs to red
    buttonFlag = FALSE;     // Reset button pressed flag
  }

  // Task puts itself to sleep
  queue[TASK_GREEN]->taskReady = FALSE;
}

void taskRedBlinkStart(void *data) {
  // DEBUG: Print the current state
  // xil_printf("Red blink start task\r\n");

  // Record elapsed time
  elapsedTime = systemTime - startTime;
  // xil_printf("Elapsed time: %d\r\n", elapsedTime);

  // Blink the LEDs every 500 ms
  // (100 units = 500 ms)
  if (elapsedTime % 100 == 0)
    ledState ^= RGB_RED; // Toggle the LEDs

  // Reset start time if button is pressed
  if (buttonFlag) {
    startTime = systemTime; // Update the start time
    buttonFlag = FALSE;     // Reset button pressed flag
  }

  // Set the next state
  // (1200 units = 6 s)
  if (elapsedTime >= 1200) {
    nextState = RED;
    startTime = systemTime; // Record the start time
    ledState = RGB_RED;     // Set the LEDs to red
    buttonFlag = FALSE;     // Reset button pressed flag
  }

  // Task puts itself to sleep
  queue[TASK_RED_BLINK_START]->taskReady = FALSE;
}

void taskRed(void *data) {
  // xil_printf("Red task...\r\n");

  // Record elapsed time
  elapsedTime = systemTime - startTime;

  // Set the LEDs to red
  ledState = RGB_RED;

  // Set the next state
  // (800 units = 4 s)
  if (elapsedTime >= 800) {
    nextState = FLASH_RED_END;
    startTime = systemTime; // Record the start time
    ledState = RGB_RED;     // Set the LEDs to red
  }

  // Task puts itself to sleep
  queue[TASK_RED]->taskReady = FALSE;
}

void taskRedBlinkEnd(void *data) {
  // DEBUG: Print the current state
  // xil_printf("Red blink stop task...\r\n");

  // Record elapsed time
  elapsedTime = systemTime - startTime;

  // Blink the LEDs every 500 ms
  // (100 units = 500 ms)
  if (elapsedTime % 100 == 0)
    ledState ^= RGB_RED; // Toggle the LEDs

  // Set the next state
  // (1200 units = 6 s)
  if (elapsedTime >= 1200) {
    nextState = GREEN;
    startTime = systemTime; // Record the start time
    ledState = RGB_GREEN;   // Set the LEDs to green
  }

  // Task puts itself to sleep
  queue[TASK_RED_BLINK_END]->taskReady = FALSE;
}

//===---------------------------------------------------------------------===//
// Interrupt Service Routines
//===---------------------------------------------------------------------===//

void timer_ISR(void *CallBackRef, u8 TmrCtrNumber) {
  // Increment system time
  // 1 unit = 5 ms
  systemTime++;

  // Get instance of the timer linked to the interrupt
  XTmrCtr *InstancePtr = (XTmrCtr *)CallBackRef;

  // Check if the timer counter has expired,
  // then set taskChoose to be ready
  if (XTmrCtr_IsExpired(InstancePtr, TmrCtrNumber)) {
    queue[TASK_CHOOSE]->taskReady = TRUE;
  }
}

void button_ISR(void *InstancePtr) {
  XGpio *gpio = (XGpio *)InstancePtr;

  // Set button pressed flag on rising edge only
  if (XGpio_DiscreteRead(gpio, XGPIO_IR_CH2_MASK) & 0xF) {
    (state == GREEN || state == FLASH_RED_START) && (buttonFlag = TRUE);
  }

  // Clear the interrupt
  // This will clear both the rising and falling edge interrupts
  XGpio_InterruptClear(&btnGpio, XGPIO_IR_CH2_MASK);
}

//===---------------------------------------------------------------------===//
// Platform Initialization
//===---------------------------------------------------------------------===//

int platform_init() {
  int status = XST_FAILURE;

  // Initialize the GPIO instances
  status = XGpio_Initialize(&btnGpio, XPAR_XGPIO_0_BASEADDR);
  if (status != XST_SUCCESS) {
    xil_printf("Failed to initialize GPIO_0! Execution stopped.\n");
    executionFailed();
  }

  status = XGpio_Initialize(&ledGpio, XPAR_XGPIO_1_BASEADDR);
  if (status != XST_SUCCESS) {
    xil_printf("Failed to initialize GPIO_1! Execution stopped.\n");
    executionFailed();
  }

  // Set GPIO_0 channel 2 as input
  XGpio_SetDataDirection(&btnGpio, XGPIO_CHANNEL_2, 0xF);

  // Set GPIO_1 channel 1 as output
  XGpio_SetDataDirection(&ledGpio, XGPIO_CHANNEL_1, 0x0);

  // Enable GPIO interrupts
  XGpio_InterruptEnable(&btnGpio, XGPIO_IR_CH2_MASK);
  XGpio_InterruptGlobalEnable(&btnGpio);

  // Initialize the timer counter instance
  status = XTmrCtr_Initialize(&Timer, XPAR_XTMRCTR_0_BASEADDR);
  if (status != XST_SUCCESS) {
    xil_printf("Failed to initialize the timer! Execution stopped.\n");
    executionFailed();
  }

  // Verifies the specified timer is setup correctly in hardware/software
  status = XTmrCtr_SelfTest(&Timer, XTC_TIMER_0);
  if (status != XST_SUCCESS) {
    xil_printf("Testing timer operation failed! Execution stopped.\n");
    executionFailed();
  }

  // Set the handler (function pointer) that we want to execute when the
  // interrupt occurs
  XTmrCtr_SetHandler(&Timer, timer_ISR, &Timer);

  // Set our timer options (setting TCSR register indirectly through Xil API)
  u32 timerConfig = XTC_INT_MODE_OPTION |   // enable interrupts
                    XTC_DOWN_COUNT_OPTION | // count down mode
                    XTC_AUTO_RELOAD_OPTION; // auto-reload mode
  XTmrCtr_SetOptions(&Timer, XTC_TIMER_0, timerConfig);

  // Set what value the timer should reset/init to (setting TLR indirectly)
  XTmrCtr_SetResetValue(&Timer, XTC_TIMER_0, LOAD_VALUE);

  // Start the timer
  XTmrCtr_Start(&Timer, XTC_TIMER_0);

  // Initialize the interrupt controller instance
  status = XIntc_Initialize(&InterruptController, XPAR_XINTC_0_BASEADDR);
  if (status != XST_SUCCESS) {
    xil_printf(
        "Failed to initialize the interrupt controller! Execution stopped.\n");
    executionFailed();
  }

  // Connect a timer handler that will be called when a timer interrupt occurs
  status = XIntc_Connect(&InterruptController, XPAR_FABRIC_AXI_TIMER_0_INTR,
                         (XInterruptHandler)XTmrCtr_InterruptHandler,
                         (void *)&Timer);
  if (status != XST_SUCCESS) {
    xil_printf("Failed to connect timer handler! Execution stopped.\n");
    executionFailed();
  }

  // Connect a GPIO handler that will be called when a button interrupt occurs
  status = XIntc_Connect(&InterruptController, XPAR_FABRIC_XGPIO_0_INTR,
                         (XInterruptHandler)button_ISR, (void *)&btnGpio);
  if (status != XST_SUCCESS) {
    xil_printf("Failed to connect button handler! Execution stopped.\n");
    executionFailed();
  }

  // Enable interrupts and the exception table
  XIntc_Enable(&InterruptController, XPAR_FABRIC_AXI_TIMER_0_INTR);
  XIntc_Enable(&InterruptController, XPAR_FABRIC_XGPIO_0_INTR);

  // Start the interrupt controller
  status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
  if (status != XST_SUCCESS) {
    xil_printf("Failed to start interrupt controller! Execution stopped.\n");
    executionFailed();
  }

  // Register the interrupt controller handler with the exception table,
  // and enable exceptions
  Xil_ExceptionInit();
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                               (Xil_ExceptionHandler)XIntc_InterruptHandler,
                               &InterruptController);
  Xil_ExceptionEnable();

  return XST_SUCCESS;
}

//===---------------------------------------------------------------------===//
// Helper Functions
//===---------------------------------------------------------------------===//

void executionFailed() {
  // Display all red LEDs if fail state occurs
  XGpio_WriteReg(XPAR_XGPIO_1_BASEADDR, XGPIO_DATA_OFFSET, RGB_RED);

  // Trap the program in an infinite loop
  while (1)
    ;
}

// Optional interrupt disable function
void TmrCtrDisableIntr(XIntc *IntcInstancePtr, u16 IntrId) {
  // Disable the interrupt for the timer counter
  XIntc_Disable(IntcInstancePtr, IntrId);

  return;
}
```

