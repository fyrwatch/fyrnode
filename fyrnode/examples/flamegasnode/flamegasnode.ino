// ===========================================================================
// Copyright (C) 2020 Manish Meganathan, Mariyam A.Ghani. All Rights Reserved.
// 
// This file is part of the FyrNode library.
// No part of the FyrNode library can not be copied and/or distributed 
// without the express permission of Manish Meganathan and Mariyam A.Ghani
// ===========================================================================
// Code for sensor node with a SEN16 flame sensor and a MQ2 gas sensor
// ===========================================================================
// Written for NodeMCU 1.0 ESP-12E board
// ===========================================================================

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