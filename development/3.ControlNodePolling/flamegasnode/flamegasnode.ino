//*************************************************************
//* Copyright (C) 2020 Manish Meganathan 757manish@gmail.com
//* 
//* This file is part of FyrWatch and FyrMesh.
//* 
//* FyrWatch can not be copied and/or distributed without the 
//* express permission of Manish Meganathan and Mariyam A.Ghani
//*************************************************************
//*************************************************************
// Code for sensor node to read and transmit data from a 
// flame (SEN16) and gas (MQ2) sensors when hit 
// with a ping request.
//
// Serial Baud Rate - 38400
// Flame Dout pin attached to D1.
// Gas Aout pin attached to A0.
//
// Written for NodeMCU 1.0 ESP-12E board
//*************************************************************

#include "painlessMesh.h"
#include "ArduinoJson.h"

#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

#define GAS A0
#define FLAME D1

painlessMesh mesh;
Scheduler userScheduler;
String configID = "flame-gas-sen16-mq2";


void respondping(String &pingID)
{
    StaticJsonDocument<256> message;
    String msg;

    message["type"] = "pingresponse";
    message["origin"] = mesh.getNodeId();
    message["reach"] = "unicast-0";

    int g = analogRead(GAS);
    int f = digitalRead(FLAME);

    message["data"]["sensors"]["config"] = configID; 
    message["data"]["sensors"]["gas"] = g;
    message["data"]["sensors"]["flame"] = f;
    message["data"]["ping"] = pingID;

    serializeJson(message, msg);
    uint32_t CONTROL_NODE = 3160379558;
    mesh.sendSingle(CONTROL_NODE, msg);
}

//painlessMesh Callback for recieved messages
void receivedCallback(uint32_t from, String &recievedmsg) 
{
    Serial.println(recievedmsg);

    StaticJsonDocument<256> recieved;
    deserializeJson(recieved, recievedmsg);
    String type = recieved["type"];  

    if (type == "pingrequest") 
    {
        String pingID = recieved["data"]["ping"];
        respondping(pingID);
    }
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
    pinMode(FLAME, INPUT);

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
