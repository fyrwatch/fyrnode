
//*************************************************************
//* Copyright (C) 2020 Manish Meganathan 757manish@gmail.com
//* 
//* This file is part of FyrWatch and FyrMesh.
//* 
//* FyrWatch can not be copied and/or distributed without the 
//* express permission of Manish Meganathan and Mariyam A.Ghani
//*************************************************************
//*************************************************************
// Code for mesh node that simply listens to the mesh and logs
// all recieved messages to the serial
//
// Serial Baud Rate - 38400
//
// Written for NodeMCU 1.0 ESP-12E board
//*************************************************************

#include "painlessMesh.h"
#include "ArduinoJson.h"

#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

painlessMesh mesh;
Scheduler userScheduler;

//painlessMesh Callback for recieved messages
void receivedCallback(uint32_t from, String &recievedmsg) 
{
    Serial.println(recievedmsg);
}

//painlessMesh Callback for new connections to the mesh
void newConnectionCallback(uint32_t nodeID) 
{
    StaticJsonDocument<256> message;
    String msg;

    message["type"] = "meshupdate";
    message["origin"] = mesh.getNodeId();
    message["reach"] = "internal-0";

    message["data"]["change"] = "newconnection";
    message["data"]["message"] = "New Node on Mesh";
    message["data"]["newnode"] = nodeID;

    serializeJson(message, msg);
    Serial.println(msg);
}

//painlessMesh Callback for modified connection messages
void changedConnectionCallback() 
{
    StaticJsonDocument<256> message;
    String msg;

    message["type"] = "meshupdate";
    message["origin"] = mesh.getNodeId();
    message["reach"] = "internal-0";

    message["data"]["change"] = "changedconnection";
    message["data"]["message"] = "Mesh Connections Modified";

    serializeJson(message, msg);
    Serial.println(msg);
}

//painlessMesh Callback for node time adjustment messages
void nodeTimeAdjustedCallback(int32_t offset) 
{
    StaticJsonDocument<256> message;
    String msg;

    message["type"] = "meshupdate";
    message["origin"] = mesh.getNodeId();
    message["reach"] = "internal-0";

    message["data"]["change"] = "nodetimeadjust";
    message["data"]["message"] = "Node Time Adjusted";
    message["data"]["nodetime"] = mesh.getNodeTime();
    message["data"]["offset"] = offset;

    serializeJson(message, msg);
    Serial.println(msg);
}

void setup()
{
	Serial.begin(38400);

    mesh.setDebugMsgTypes( ERROR | STARTUP );
    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);

    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void loop()
{
	mesh.update();
}
