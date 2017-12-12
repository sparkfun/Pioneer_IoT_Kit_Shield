#include "project.h"
#include "ipc_common.h"

/* Global shared variable is static within this file. The address of this
   variable is given to the CM4 via an IPC channel. */
static uint8_t D9Button;
static uint8_t D7Button;

static volatile cy_en_ipcdrv_status_t mySysError;  /* general-purpose status/error code */

/* Required for Cy_IPC_LOCK_Init(), not actually used in this code example.
   A call to this function may not be needed in future IPC driver revisions. */
#define LOCK_COUNT 128u

/*******************************************************************************
* Function Name: HandleError
****************************************************************************//**
*
* Placeholder function for handling IPC and other errors. Does nothing -
* implements halt on error.
*  
*******************************************************************************/
void HandleError(void)
{
    /* for now, just halt on error */
    while(1)
    {
      Cy_GPIO_Clr(Red_LED_0_PORT, Red_LED_0_NUM);
    }
}

/*******************************************************************************
* Function Name: main
****************************************************************************//**
*
* The main function for the Cortex-M0+ CPU does the following:<br>
*  Initialization:<br>
*   - Enables the other CPU.<br>
*   - Initializes IPC channel for sharing a variable with the other CPU.<br>
*  Do forever loop:<br>
*   - Increments the 4 LS bits of the shared variable, without changing the 4 MS
*     bits.<br>
*   - Waits for the 4 MS bits of the shared variable to equal the 4 LS bits. The
*     other CPU sets the 4 MS bits to be equal to the 4 LS bits.
*
*******************************************************************************/
int main(void)
{
  IPC_STRUCT_Type *D9IpcHandle; /* handle for the IPC channel being used */
  IPC_STRUCT_Type *D7IpcHandle; /* handle for the IPC channel being used */
  
  /* Always use lock/release functions to access the shared variable; do
     computations on a local copy of the shared variable. */
  uint8_t copy;
  uint32 gpioRes;
  Cy_GPIO_Set(Red_LED_0_PORT, Red_LED_0_NUM);
  UART_Start();

  __enable_irq(); /* Enable global interrupts. */
  /* Enable CM4.  CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout is changed. */
  Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR); 

  /* Place your initialization/startup code here (e.g. MyInst_Start()) */
  /* Do the first read of the shared variable, preparatory to modifying and
     writing it for the first time in the main loop.
     This initial read must be done before notifying the CM4. */
  copy = 0;

  /* Initialize the IPC lock subsystem. A call to this function may not be needed
     in future IPC driver revisions. */
  //if (Cy_IPC_LOCK_Init(LOCK_COUNT, myArray) != CY_IPC_LOCK_SUCCESS) HandleError();
  
  /* Get the handle for the IPC channel to be used. */
  D9IpcHandle = Cy_IPC_Drv_GetIpcBaseAddress(7);
  D7IpcHandle = Cy_IPC_Drv_GetIpcBaseAddress(8);
  /* Notify the CM4 with the shared memory address. There is no interrupt
     associated with this notification.
     Wait for channel to be available, then acquire the lock for it, in a
     single atomic operation; and send the address. */
  while(Cy_IPC_Drv_SendMsgPtr(D9IpcHandle, CY_IPC_NO_NOTIFICATION, &D9Button) != CY_IPC_DRV_SUCCESS);
  /* Wait for release, which indicates other CPU has read the pointer value. */
  while(Cy_IPC_Drv_IsLockAcquired(D9IpcHandle));
  while(Cy_IPC_Drv_SendMsgPtr(D7IpcHandle, CY_IPC_NO_NOTIFICATION, &D7Button) != CY_IPC_DRV_SUCCESS);
  /* Wait for release, which indicates other CPU has read the pointer value. */
  while(Cy_IPC_Drv_IsLockAcquired(D7IpcHandle));

  for(;;)
  {
    gpioRes = Cy_GPIO_Read(D9_0_PORT, D9_0_NUM);
    if (gpioRes == 0)
    {
      copy = 0x01;
      mySysError = (cy_en_ipcdrv_status_t)WriteSharedVar(&D9Button, copy, D9_SEMAPHORE);
      CyDelay(50);
      while (gpioRes == 0)
      {
        gpioRes = Cy_GPIO_Read(D9_0_PORT, D9_0_NUM);
      }
    } 
    gpioRes = Cy_GPIO_Read(D7_0_PORT, D7_0_NUM);
    if (gpioRes == 0)
    {
      copy = 0x10;
      mySysError = (cy_en_ipcdrv_status_t)WriteSharedVar(&D7Button, copy, D7_SEMAPHORE);
      CyDelay(50);
      while (gpioRes == 0)
      {
        gpioRes = Cy_GPIO_Read(D7_0_PORT, D7_0_NUM);
      }
    } 
  }
}

/* [] END OF FILE */
