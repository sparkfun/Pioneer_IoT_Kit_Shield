#include "project.h"
#include "ipc_common.h"

static volatile cy_en_ipcdrv_status_t mySysError;  /* general-purpose status/error code */


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
        Cy_GPIO_Clr(Blue_LED_0_PORT, Blue_LED_0_NUM);
    }
}

/*******************************************************************************
* Function Name: main
****************************************************************************//**
*
* The main function for the Cortex-M4 CPU does the following:<br>
*  Initialization:<br>
*   - Initializes IPC channel for sharing a variable with the other CPU.<br>
*  Do forever loop:<br>
*   - Waits for the 4 MS bits of the shared variable to not equal the 4 LS bits.
*     The other CPU increments the 4 LS bits.<br>
*   - sets the 4 MS bits to equal the 4 LS bits.
*
*******************************************************************************/
int main(void)
{
  IPC_STRUCT_Type *D9IpcHandle;
  IPC_STRUCT_Type *D7IpcHandle;

  /* A global shared variable is defined in the codefor the other CPU.
     Its address is given to the this CPU via an IPC channel. */
  uint8_t *D9Button;
  uint8_t *D7Button;
  
  Cy_GPIO_Set(Blue_LED_0_PORT, Blue_LED_0_NUM);

  __enable_irq(); /* Enable global interrupts. */

  D9IpcHandle = Cy_IPC_Drv_GetIpcBaseAddress(7);
  D7IpcHandle = Cy_IPC_Drv_GetIpcBaseAddress(8);

  /* Read the shared memory address, from the other CPU. Wait for success,
     which indicates that (1) the sending CPU acquired the lock, and (2) this
     CPU read the pointer. */
  while (Cy_IPC_Drv_ReadMsgPtr(D9IpcHandle, (void *)&D9Button) != CY_IPC_DRV_SUCCESS);
  /* Release the lock. This indicates to the other CPU that the read is 
     successfully complete. */
  mySysError = Cy_IPC_Drv_LockRelease(D9IpcHandle, CY_IPC_NO_NOTIFICATION);
  if (mySysError != CY_IPC_DRV_SUCCESS) HandleError();
  while (Cy_IPC_Drv_ReadMsgPtr(D7IpcHandle, (void *)&D7Button) != CY_IPC_DRV_SUCCESS);
  /* Release the lock. This indicates to the other CPU that the read is 
     successfully complete. */
  mySysError = Cy_IPC_Drv_LockRelease(D7IpcHandle, CY_IPC_NO_NOTIFICATION);
  if (mySysError != CY_IPC_DRV_SUCCESS) HandleError();
  for(;;)
  {
    /* Always use lock/release functions to access the shared variable;
       do computations on a local copy of the shared variable. */
    uint8_t copy = 0;
    mySysError = (cy_en_ipcdrv_status_t)ReadSharedVar(D9Button, &copy);
    if (copy != 0)
    {
      UART_PutString("GET /gpio/2/0 HTTP/1.1\r");
      while (UART_GetNumLeftToTransmit());
      copy = 0;
      (cy_en_ipcdrv_status_t)WriteSharedVar(D9Button, copy);
    }    
    copy = 0;
    mySysError = (cy_en_ipcdrv_status_t)ReadSharedVar(D7Button, &copy);
    if (copy != 0)
    {
      UART_PutString("GET /gpio/2/1 HTTP/1.1\r");
      while (UART_GetNumLeftToTransmit());
      copy = 0;
      (cy_en_ipcdrv_status_t)WriteSharedVar(D7Button, copy);
    } 
  }
}

/* [] END OF FILE */