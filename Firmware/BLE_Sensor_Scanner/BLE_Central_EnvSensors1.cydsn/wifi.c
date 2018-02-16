/****************************************************************************
wifi.c
WiFi support functionality
Mike Hord @ SparkFun Electronics
3 Jan 2018
https://github.com/sparkfun/Pioneer_Kit_Shield

Functions for XBee WiFi support

Resources:
No additional resources required

Development environment specifics:
PSoC Creator 4.2
CY8KIT-062-BLE

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!
****************************************************************************/

#include "wifi.h"
#include "project.h"
#include "stdio.h"

void XBeeWiFiInit()
{
  // Configure the XBee connection settings.
  char buffer[40];       // Create a buffer for our commands
  char ssid[] = "sparkfun-guest";
  char ts_ip[] = "184.106.153.149";  // thingspeak.com IP address
  char ssid_pw[] = "sparkfun6333";
  int dest_port = 80;
  char encrypt_mode = WPA2;
  
  XBEE_UART_PutString("+++");  // Enter AT command mode for the XBee
  CyDelay(1000);          // A pause is required after the +++ command
  
  XBEE_UART_PutString("ATRE\r"); // Reset module to default settings.
  CyDelay(2000);            // Wait two seconds for reset to finish
  
  XBEE_UART_PutString("+++");  // Enter AT command mode for the XBee
  CyDelay(1000);          // A pause is required after the +++ command
  
  sprintf(buffer, "ATID%s\r", ssid); // Set up the XBee to use your SSID
  XBEE_UART_PutString(buffer);
  
  sprintf(buffer, "ATPK%s\r", ssid_pw); // Set up the password for your SSID
  XBEE_UART_PutString(buffer);
  
  XBEE_UART_PutString("ATIP1\r");  // TCP mode enable (default is UDP)
  
  sprintf(buffer, "ATEE%d\r", encrypt_mode); // Set up your encryption mode
  XBEE_UART_PutString(buffer);
  
  sprintf(buffer, "ATDL%s\r", ts_ip); // Set the destination IP
  XBEE_UART_PutString(buffer);
  
  sprintf(buffer, "ATDE%x\r", dest_port); // Set the destination port
  XBEE_UART_PutString(buffer);
    
  XBEE_UART_PutString("ATTM5\r");   // Set the TCP Client timeout to 500ms to send
                               //  packet a half-second after completion to
                               //  ensure that packet is complete
  
  XBEE_UART_PutString("ATWR\r");    // Write settings to flash memory.
}