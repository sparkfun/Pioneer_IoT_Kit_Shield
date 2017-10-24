#include "project.h"
#include "ipc_common.h"

/* Global shared variable is static within this file. The address of this
   variable is given to the CM4 via an IPC channel. */
static uint8_t yellowButton;
static uint8_t whiteButton;

static volatile cy_en_ipcdrv_status_t mySysError;  /* general-purpose status/error code */

/* Required for Cy_IPC_LOCK_Init(), not actually used in this code example.
   A call to this function may not be needed in future IPC driver revisions. */
#define LOCK_COUNT 128u
static uint32_t myArray[4];


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
      Cy_GPIO_Clr(Red_LED_0);
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
  IPC_STRUCT_Type *yellowIpcHandle; /* handle for the IPC channel being used */
  IPC_STRUCT_Type *whiteIpcHandle; /* handle for the IPC channel being used */
  
  /* Always use lock/release functions to access the shared variable; do
     computations on a local copy of the shared variable. */
  uint8_t copy;
  uint32 gpioRes;
  Cy_GPIO_Set(Red_LED_0);
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
  if (Cy_IPC_LOCK_Init(LOCK_COUNT, myArray) != CY_IPC_LOCK_SUCCESS) HandleError();
  
  /* Get the handle for the IPC channel to be used. */
  yellowIpcHandle = Cy_IPC_DRV_GetIpcBaseAddress(MY_IPC_CHANNEL);
  whiteIpcHandle = Cy_IPC_DRV_GetIpcBaseAddress(MY_IPC_CHANNEL);
  /* Notify the CM4 with the shared memory address. There is no interrupt
     associated with this notification.
     Wait for channel to be available, then acquire the lock for it, in a
     single atomic operation; and send the address. */
  while(Cy_IPC_DRV_SendMsgPtr(yellowIpcHandle, CY_IPC_NO_NOTIFIFICATION, &yellowButton) != CY_IPC_DRV_SUCCESS);
  /* Wait for release, which indicates other CPU has read the pointer value. */
  while(Cy_IPC_DRV_GetLockStatus(yellowIpcHandle) == CY_IPC_DRV_LOCKED);
  while(Cy_IPC_DRV_SendMsgPtr(whiteIpcHandle, CY_IPC_NO_NOTIFIFICATION, &whiteButton) != CY_IPC_DRV_SUCCESS);
  /* Wait for release, which indicates other CPU has read the pointer value. */
  while(Cy_IPC_DRV_GetLockStatus(whiteIpcHandle) == CY_IPC_DRV_LOCKED);

  for(;;)
  {
    gpioRes = Cy_GPIO_Read(Yellow_Btn_0);
    if (gpioRes == 0)
    {
      copy = 0x01;
      mySysError = (cy_en_ipcdrv_status_t)WriteSharedVar(&yellowButton, copy);
      while (gpioRes == 0)
      {
        gpioRes = Cy_GPIO_Read(Yellow_Btn_0);
      }
      //if (mySysError != (cy_en_ipcdrv_status_t)CY_IPC_LOCK_SUCCESS) HandleError();
    } 
    gpioRes = Cy_GPIO_Read(White_Btn_0);
    if (gpioRes == 0)
    {
      copy = 0x10;
      mySysError = (cy_en_ipcdrv_status_t)WriteSharedVar(&whiteButton, copy);
      while (gpioRes == 0)
      {
        gpioRes = Cy_GPIO_Read(White_Btn_0);
      }
      //if (mySysError != (cy_en_ipcdrv_status_t)CY_IPC_LOCK_SUCCESS) HandleError();
    } 
  }
}

/* [] END OF FILE */
