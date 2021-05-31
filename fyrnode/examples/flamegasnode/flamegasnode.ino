/*
===========================================================================
MIT License

Copyright (c) 2021 Manish Meganathan, Mariyam A.Ghani

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
===========================================================================
Code for sensor node with a SEN16 flame sensor and a MQ2 gas sensor
===========================================================================
Written for NodeMCU 1.0 ESP-12E board
===========================================================================
*/

#include "fyrnode.h"

String MESH_SSID = "whateverYouLike";
String MESH_PSWD = "somethingSneaky";
uint16_t MESH_PORT = 5555;

int DHTTYP = 0;		    // DHT attached to None
int DHTPIN = 99;		// DHT attached at None

int GASTYP = 2;			// GAS attached to MQ2 Sensor
int GASPIN = 17;		// GAS attached at Pin A0

int FLMTYP = 16;		// FLM attached to SEN16 Sensor
int FLMPIN = 5;		    // FLM attached at Pin D1 (GPIO5)

bool PINGER = false;	//PINGER attached = false
int PINGERPIN = 99;		//PINGER attached at None

int CONNECTLEDPIN = 16;         // CONNECTLEDPIN attached at Pin D0 (LED_BUILTIN)
uint32_t SERIALBAUD = 115200;   // SERIALBAUD rate is 115200 bps

FyrNode meshnode;

void setup() 
{
    meshnode.begin();
}

void loop() 
{
    meshnode.update();
}