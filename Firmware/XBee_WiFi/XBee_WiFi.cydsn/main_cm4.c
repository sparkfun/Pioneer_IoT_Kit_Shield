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
        Cy_GPIO_Clr(Blue_LED_0);
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
  IPC_STRUCT_Type *yellowIpcHandle;
  IPC_STRUCT_Type *whiteIpcHandle;

  /* A global shared variable is defined in the codefor the other CPU.
     Its address is given to the this CPU via an IPC channel. */
  uint8_t *yellowButton;
  uint8_t *whiteButton;
  
  Cy_GPIO_Set(Blue_LED_0);

  __enable_irq(); /* Enable global interrupts. */

  yellowIpcHandle = Cy_IPC_DRV_GetIpcBaseAddress(MY_IPC_CHANNEL);
  whiteIpcHandle = Cy_IPC_DRV_GetIpcBaseAddress(MY_IPC_CHANNEL);

  /* Read the shared memory address, from the other CPU. Wait for success,
     which indicates that (1) the sending CPU acquired the lock, and (2) this
     CPU read the pointer. */
  while (Cy_IPC_DRV_ReadMsgPtr(yellowIpcHandle, (void *)&yellowButton) != CY_IPC_DRV_SUCCESS);
  /* Release the lock. This indicates to the other CPU that the read is 
     successfully complete. */
  mySysError = Cy_IPC_DRV_Release(yellowIpcHandle, CY_IPC_NO_NOTIFIFICATION);
  if (mySysError != CY_IPC_DRV_SUCCESS) HandleError();
  while (Cy_IPC_DRV_ReadMsgPtr(whiteIpcHandle, (void *)&whiteButton) != CY_IPC_DRV_SUCCESS);
  /* Release the lock. This indicates to the other CPU that the read is 
     successfully complete. */
  mySysError = Cy_IPC_DRV_Release(whiteIpcHandle, CY_IPC_NO_NOTIFIFICATION);
  if (mySysError != CY_IPC_DRV_SUCCESS) HandleError();
  for(;;)
  {
    /* Always use lock/release functions to access the shared variable;
       do computations on a local copy of the shared variable. */
    uint8_t copy = 0;
    mySysError = (cy_en_ipcdrv_status_t)ReadSharedVar(yellowButton, &copy);
    if (mySysError != (cy_en_ipcdrv_status_t)CY_IPC_LOCK_SUCCESS) HandleError();
    if (copy != 0)
    {
      UART_PutString("GET /gpio/2/0 HTTP/1.1\r");
      while (UART_GetNumLeftToTransmit());
      copy = 0;
      mySysError = (cy_en_ipcdrv_status_t)WriteSharedVar(yellowButton, copy);
      if (mySysError != (cy_en_ipcdrv_status_t)CY_IPC_LOCK_SUCCESS) HandleError();
    }    
    copy = 0;
    mySysError = (cy_en_ipcdrv_status_t)ReadSharedVar(whiteButton, &copy);
    if (mySysError != (cy_en_ipcdrv_status_t)CY_IPC_LOCK_SUCCESS) HandleError();
    if (copy != 0)
    {
      UART_PutString("GET /gpio/2/1 HTTP/1.1\r");
      while (UART_GetNumLeftToTransmit());
      copy = 0;
      mySysError = (cy_en_ipcdrv_status_t)WriteSharedVar(whiteButton, copy);
      if (mySysError != (cy_en_ipcdrv_status_t)CY_IPC_LOCK_SUCCESS) HandleError();
    } 
  }
}

/* [] END OF FILE */