// ===========================================================================
// Copyright (C) 2020 Manish Meganathan, Mariyam A.Ghani. All Rights Reserved.
// 
// This file is part of the FyrNode library.
// No part of the FyrNode library can not be copied and/or distributed 
// without the express permission of Manish Meganathan and Mariyam A.Ghani
// ===========================================================================
// Code for control node to forward all recived messages to its software 
// serial port and to send ping request to the mesh when a button is pressed. 
//
// Serial Baud Rate - 38400 (HW & SW Serial)
// Software Serial Pins - D5, D6 (GPIO14 and GPIO12)
// Pinger Button is attached to D0 (GPIO16)
//
// Written for NodeMCU 1.0 ESP-12E board
// ===========================================================================

#include "fyrnode.h"

String MESH_SSID = "whateverYouLike";
String MESH_PSWD = "somethingSneaky";
int MESH_PORT = 5555;

int DHTTYP = 0;		    //DHT Attached to DHT11 Sensor
int DHTPIN = 99;		//DHT Attached at Pin D1 (GPIO5)

int GASTYP = 0;			//GAS Attached to None
int GASPIN = 99;		//GAS Attached at None

int FLMTYP = 0;			//FLM Attached to None
int FLMPIN = 99;		//FLM Attached at None

bool PINGER = true;	    //PINGER Attached = false
int PINGERPIN = 16;		//PINGER Attached at None

bool CONNECT = false;	//CONNECT Attached = false
int CONNECTPIN = 99;	//CONNECT Attached at None

int SRLRX = 14;			//SRLRX Attached to Pin D5 (GPIO14)
int SRLTX = 12;			//SRLTX Attached to Pin D6 (GPIO12)

FyrNodeControl controlnode;
void setup(){controlnode.begin();}
void loop(){controlnode.update();}