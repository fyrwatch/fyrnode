// ===========================================================================
// Copyright (C) 2020 Manish Meganathan, Mariyam A.Ghani. All Rights Reserved.
// 
// This file is part of the FyrNode library.
// No part of the FyrNode library can not be copied and/or distributed 
// without the express permission of Manish Meganathan and Mariyam A.Ghani
// ===========================================================================
// Code for control node
// ===========================================================================
// Written for NodeMCU 1.0 ESP-12E board
// ===========================================================================

#include "fyrnode.h"

String MESH_SSID = "whateverYouLike";
String MESH_PSWD = "somethingSneaky";
uint16_t MESH_PORT = 5555;

int DHTTYP = 0;		    // DHT attached to None
int DHTPIN = 99;		// DHT attached at None

int GASTYP = 0;			// GAS attached to None
int GASPIN = 99;		// GAS attached at None

int FLMTYP = 0;			// FLM attached to None
int FLMPIN = 99;		// FLM attached at None

bool PINGER = true;	    //PINGER attached = true
int PINGERPIN = 5;		//PINGER attached at Pin D1 (GPIO5)

int CONNECTLEDPIN = 16;         // CONNECTLEDPIN attached at Pin D0 (LED_BUILTIN)
uint32_t SERIALBAUD = 115200;    // SERIALBAUD rate is 38400 bps

FyrNodeControl controlnode;

void setup() 
{
    controlnode.begin();
}

void loop() 
{
    controlnode.update();
}