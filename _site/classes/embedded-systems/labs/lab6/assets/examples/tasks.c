#include <xil_printf.h>
#include <xparameters.h>
#include <xtmrctr.h>
#include <xtmrctr_l.h>
#include <xil_exception.h>
#include <xil_printf.h>
#include <xintc.h>

#define TMRCTR_DEVICE_ID XPAR_TMRCTR_0_DEVICE_ID
#define TMRCTR_INTERRUPT_ID XPAR_INTC_0_TMRCTR_0_VEC_ID
#define INTC_DEVICE_ID XPAR_INTC_0_DEVICE_ID

#define LOAD_VALUE 41666667

// Function Prototypes
// Tasks
void taskChoose(void *data);
void taskGreen(void *data);
void taskRedBlinkStart(void *data);
void taskRed(void *data);
void taskRedBlinkStop(void *data);
// Timer Interrupt Service Routine
void Timer_ISR(void *CallBackRef, u8 TmrCtrNumber);
// Helper functions
int platform_init();
void TmrCtrDisableIntr(XIntc *IntcInstancePtr, u16 IntrId);
void executionFailed();

// Enumerate type for the states
typedef enum
{
  GREEN,
  TASK_RED_BLINK_START,
  TASK_RED,
  TASK_RED_BLINK_STOP
} state_t;

// Enumerated type for the tasks
typedef enum
{
  TASK_CHOOSE,
  TASK_GREEN,
  TASK_RED_BLINK_START,
  TASK_RED,
  TASK_RED_BLINK_STOP,
  MAX_TASKS
} task_t;

// Task Control Block (TCB) structure
typedef struct
{
  (*taskPtr)(void *);
  void *taskDataPtr;
  unsigned taskReady;
} TCB_t;

// Hardware instances
XIntc InterruptController; // Instance of the Interrupt Controller
XTmrCtr Timer;             // Instance of the Timer
Xgpio Gpio_0;              // Instance of the AXI_GPIO_0 GPIO driver
Xgpio Gpio_1;              // Instance of the AXI_GPIO_1 GPIO driver

// Task queue
TCB_t queue[MAX_TASKS];

// Global variables
state_t state, nextState; // State variables
u32 systemTime = 0;       // System time: 1 unit = 5 ms
u32 startTime = 0;        // Start time of the current state
u32 elapsedTime = 0;      // Elapsed time since last state transition
u32 ledState = 0;         // Current state of the LEDs

int main(int argc, char const *argv[])
{
  // Initialize the platform:
  // Setup AXI_GPIO_0
  volatile u32 *buttonData = (u32 *)(XPAR_GPIO_0_BASEADDR + 0x8);
  volatile u32 *buttonTri = (u32 *)(XPAR_GPIO_0_BASEADDR + 0xC);
  buttonTri = 0xF; // Set as input
  // Setup AXI_GPIO_1
  volatile u32 *rgbLedsData = (u32 *)(XPAR_GPIO_1_BASEADDR);
  volatile u32 *rgbLedsTri = (u32 *)(XPAR_GPIO_1_BASEADDR + 0x4);
  rgbLedsTri = 0x0; // Set as output
  // Setup the interrupt controller and the timer
  platform_init();

  return 0;
}

void taskChoose(void *data)
{
  xil_printf("Choose task\n");
}

void taskGreen(void *data)
{
  xil_printf("Green task\n");
}

void taskRedBlinkStart(void *data)
{
  xil_printf("Red blink start task\n");
}

void taskRed(void *data)
{
  xil_printf("Red task\n");
}

void taskRedBlinkStop(void *data)
{
  xil_printf("Red blink stop task\n");
}

void timer_ISR(void *CallBackRef, u32 TmrCtrNumber)
{
  // Get instance of the timer linked to the interrupt
  XTmrCtr *InstancePtr = (XTmrCtr *)CallBackRef;

  // Acknowledge the interrupt
  XTmrCtr_ClearStats(InstancePtr, TmrCtrNumber);

  // Set taskChoose to be ready
  queue[TASK_CHOOSE].taskReady = 1;
}

int platform_init()
{
  int status = XST_FAILURE;

  // Initialize the timer counter instance
  status = XTmrCtr_Initialize(&Timer, TMRCTR_DEVICE_ID);
  if (status != XST_SUCCESS)
  {
    xil_printf("Failed to initialize the timer! Execution stopped.\n");
    executionFailed();
  }

  // Verifies the specified timer is setup correctly in hardware/software
  status = XTmrCtr_SelfTest(&Timer, TIMER_0);
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
                         (void *)&Timer);
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

  // Enable exceptions
  Xil_ExceptionEnable();

  // Set the handler (function pointer) that we want to execute when the
  // interrupt occurs
  XTmrCtr_SetHandler(&Timer, Timer_ISR, &Timer);

  // Set our timer options (setting TCSR register indirectly through Xil API)
  u32 timerConfig = XTC_INT_MODE_OPTION |
                    XTC_DOWN_COUNT_OPTION |
                    XTC_AUTO_RELOAD_OPTION;
  XTmrCtr_SetOptions(&Timer, TIMER_0, timerConfig);

  // Set what value the timer should reset/init to (setting TLR indirectly)
  XTmrCtr_SetResetValue(&TimerCounter, TIMER_0, LOAD_VALUE);

  // Start the timer
  XTmrCtr_Start(&TimerCounter, TIMER_0);
}

void executionFailed()
{
  u32 *rgbLedsData = (u32 *)(XPAR_GPIO_1_BASEADDR);
  *rgbLedsData = RGB_RED; // display all red LEDs if fail state occurs

  // trap the program in an infinite loop
  while (1)
    ;
}

// Optional demonstration on how to disable interrupt
void TmrCtrDisableIntr(XIntc *IntcInstancePtr, u16 IntrId)
{
  // Disable the interrupt for the timer counter
  XIntc_Disable(IntcInstancePtr, IntrId);

  return;
}