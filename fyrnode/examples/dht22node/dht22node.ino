// ===========================================================================
// Copyright (C) 2020 Manish Meganathan, Mariyam A.Ghani. All Rights Reserved.
// 
// This file is part of the FyrNode library.
// No part of the FyrNode library can not be copied and/or distributed 
// without the express permission of Manish Meganathan and Mariyam A.Ghani
// ===========================================================================
// Code for sensor node with a DHT22 sensor
// ===========================================================================
// Written for NodeMCU 1.0 ESP-12E board
// ===========================================================================

#include "fyrnode.h"

String MESH_SSID = "whateverYouLike";
String MESH_PSWD = "somethingSneaky";
uint16_t MESH_PORT = 5555;

int DHTTYP = 22;		// DHT attached to DHT11 Sensor
int DHTPIN = 5;			// DHT attached at Pin D1 (GPIO5)

int GASTYP = 0;			// GAS attached to None
int GASPIN = 99;		// GAS attached at None

int FLMTYP = 0;			// FLM attached to None
int FLMPIN = 99;		// FLM attached at None

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