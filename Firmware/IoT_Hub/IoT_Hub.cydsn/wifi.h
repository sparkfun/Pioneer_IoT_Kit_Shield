/****************************************************************************
wifi.h
WiFi support functionality header file
Mike Hord @ SparkFun Electronics
3 Jan 2018
https://github.com/sparkfun/Pioneer_Kit_Shield

Function declarations, definitions, etc., for XBee WiFi support

Resources:
No additional resources required

Development environment specifics:
PSoC Creator 4.2
CY8KIT-062-BLE

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!
****************************************************************************/

#ifndef __wifi_h__
#define __wifi_h__
  
#define NO_SECURITY 0
#define WPA         1
#define WPA2        2
#define WEP         3
  
void XBeeWiFiInit();
  
#endif