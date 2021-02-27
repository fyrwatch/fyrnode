"""
*************************************************************
* Copyright (C) 2020 Manish Meganathan 757manish@gmail.com
* 
* This file is part of FyrWatch and FyrMesh.
* 
* FyrWatch can not be copied and/or distributed without the 
* express permission of Manish Meganathan and Mariyam A.Ghani
*************************************************************
*************************************************************
Code for RaspberryPi to initialise a serial connection with 
mesh control node and parse those messages into meaningful
data and print it.

Serial Baud Rate - 38400
Serial Port - /dev/ttyAMA0

Written for NodeMCU 1.0 ESP-12E board
*************************************************************
"""

import serial
import json

serialport = serial.Serial(
    port='/dev/ttyAMA0', 
    baudrate='38400',
    parity=serial.PARITY_NONE, 
    stopbits=serial.STOPBITS_ONE, 
    bytesize=serial.EIGHTBITS, 
    timeout=1)

def read():
    rxdata = serialport.read_until()

    try:
        finaldata = json.loads(rxdata)
    except:
        try:
            finaldata = rxdata.decode('ascii')
        except:
            finaldata = rxdata

    if finaldata:
        print(finaldata)
        print(type(finaldata))
        print("\n")

if __name__ == '__main__':
    while 1:
        read()