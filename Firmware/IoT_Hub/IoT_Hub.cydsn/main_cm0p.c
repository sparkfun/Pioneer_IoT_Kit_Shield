/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "wifi.h"

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */

    UART_Start();
    //XBeeWiFiInit();

    for(;;)
    {
    CyDelay(15000);
    UART_PutString("GET /update?key=TLMJN63UZYSJTBXA&field1=0&field2=0&field3=0&field4=0 HTTP/1.1\r\n");
    UART_PutString("Host: api.thingspeak.com\r\n\r\n");
    }
}

/* [] END OF FILE */
