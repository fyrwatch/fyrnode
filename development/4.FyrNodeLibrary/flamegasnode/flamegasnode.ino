// ===========================================================================
// Copyright (C) 2020 Manish Meganathan, Mariyam A.Ghani. All Rights Reserved.
// 
// This file is part of the FyrNode library.
// No part of the FyrNode library can not be copied and/or distributed 
// without the express permission of Manish Meganathan and Mariyam A.Ghani
// ===========================================================================
// Code for sensor node to read and transmit data from a flame (SEN16) and 
// gas (MQ2) sensors when hit with a ping request.
//
// Serial Baud Rate - 38400
// Flame Dout pin attached to D1 (GPIO5)
// Gas Aout pin attached to A0 (PIN17)
//
// Written for NodeMCU 1.0 ESP-12E board
// ===========================================================================

#include "fyrnode.h"

String MESH_SSID = "whateverYouLike";
String MESH_PSWD = "somethingSneaky";
int MESH_PORT = 5555;

int DHTTYP = 0;		    //DHT Attached to DHT11 Sensor
int DHTPIN = 99;		//DHT Attached at Pin D1 (GPIO5)

int GASTYP = 2;			//GAS Attached to None
int GASPIN = 17;		//GAS Attached at None

int FLMTYP = 16;		//FLM Attached to None
int FLMPIN = 5;		    //FLM Attached at None

bool PINGER = false;	//PINGER Attached = false
int PINGERPIN = 99;		//PINGER Attached at None

bool CONNECT = false;	//CONNECT Attached = false
int CONNECTPIN = 99;	//CONNECT Attached at None

int SRLRX = 99;			//SRLRX Attached to None
int SRLTX = 99;			//SRLTX Attached to None

FyrNode meshnode;
void setup(){meshnode.begin();}
void loop(){meshnode.update();}