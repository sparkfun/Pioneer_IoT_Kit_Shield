#!/usr/bin/python
from bluepy.btle import Scanner, DefaultDelegate, Peripheral, UUID
import struct
import RPi.GPIO as GPIO

 
GPIO.setmode(GPIO.BCM)  # Sets up the RPi lib to use the Broadcom pin mappings
                        #  for the pin names. This corresponds to the pin names
                        #  given in most documentation of the Pi header
GPIO.setwarnings(False) # Turn off warnings that may crop up if you have the
                        #  GPIO pins exported for use via command line
GPIO.setup(2, GPIO.OUT) # Set GPIO2 as an output
GPIO.setup(3, GPIO.OUT) # Set GPIO2 as an output

class ScanDelegate(DefaultDelegate):
    btn0State = False
    btn1State = False

    def __init__(self):
        DefaultDelegate.__init__(self)

    def handleNotification(self, cHandle, data):
        print "Notification received!"
        btns = ord(data[2])
        if not self.btn0State and btns&0x01:
            self.btn0State=True
            print "btn 0 on"
            GPIO.output(2, 1)
        elif self.btn0State and not btns&0x01:
            self.btn0State = False
            print "btn 0 off"
        if not self.btn1State and btns&0x02:
            self.btn1State=True
            print "btn 1 on"
            GPIO.output(2, 0)
        elif self.btn1State and not btns&0x02:
            self.btn1State = False
            print "btn 1 off"

device = Peripheral("00:A0:50:21:81:34")
device.withDelegate(ScanDelegate())
device.writeCharacteristic(29, b"\x01\x00", withResponse=True)
device.writeCharacteristic(23, b"\xff\xff\xff\xff", withResponse=True)

while True:
    if device.waitForNotifications(1.0):
        continue

    print "Waiting..."
