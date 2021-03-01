// ===========================================================================
// Copyright (C) 2020 Manish Meganathan, Mariyam A.Ghani. All Rights Reserved.
// 
// This file is part of the FyrNode library.
// No part of the FyrNode library can not be copied and/or distributed 
// without the express permission of Manish Meganathan and Mariyam A.Ghani
// ===========================================================================

#include "fyrnode.h"
#include "Arduino.h"
#include "painlessMesh.h"
#include "ArduinoJson.h"
#include "SoftwareSerial.h"
#include "JC_Button.h"
#include "DHT.h"

extern int MESH_PORT;
extern String MESH_SSID;
extern String MESH_PSWD;

extern int DHTTYP;
extern int DHTPIN;
extern int GASTYP;
extern int GASPIN;
extern int FLMTYP;
extern int FLMPIN;

extern bool PINGER;
extern int PINGERPIN;	
extern bool CONNECT;
extern int CONNECTPIN;

extern int SRLRX;
extern int SRLTX;

painlessMesh mesh;
uint32_t CONTROLNODE;
Scheduler meshScheduler;
DHT dht(DHTPIN, DHTTYP);
Button pingerbutton(PINGERPIN);
Button connectbutton(CONNECTPIN);
SoftwareSerial serialport(SRLRX, SRLTX);

// A function to get sensor readings from the 
// DHT Sensor and fill it into the JSON document
void fillDHT(StaticJsonDocument<256> &message) 
{
    float h = dht.readHumidity();
    message["data"]["sensors"]["hum"] = h;

    float t = dht.readTemperature();
    message["data"]["sensors"]["temp"] = t;
}

// A function to get sensor readings from a
// GAS Sensor and fill it into the JSON document
void fillGAS(StaticJsonDocument<256> &message) 
{
    int g = analogRead(GASPIN);
    message["data"]["sensors"]["gas"] = g;
}

// A function to get sensor readings from a
// FLM Sensor and fill it into the JSON document
void fillFLM(StaticJsonDocument<256> &message) 
{
    int f = digitalRead(FLMPIN);
    message["data"]["sensors"]["flame"] = f;
}

// A function to respond to 'pingrequest' commands
void respondPing(String &pingID) 
{
    StaticJsonDocument<256> message;
    
    message["type"] = "pingresponse";
    message["origin"] = mesh.getNodeId();

    message["reach"]["type"] = "unicast";
    message["reach"]["set"] = CONTROLNODE;

    message["data"]["ping"] = pingID;

    if (DHTTYP > 0) {fillDHT(message);}
    if (GASTYP > 0) {fillGAS(message);}
    if (FLMTYP > 0) {fillFLM(message);}

    String msg;
    serializeJson(message, msg);
    mesh.sendSingle(CONTROLNODE, msg);
}

// A function to broadcast a 'pingrequest' command
void requestPing(String &ping)
{
    StaticJsonDocument<256> message; 
    
    message["type"] = "pingrequest";
    message["origin"] = mesh.getNodeId();

    message["reach"]["type"] = "broadcast";
    message["reach"]["set"] = "null";

    message["data"]["ping"] = ping;

    String msg;
    serializeJson(message, msg);
    mesh.sendBroadcast(msg);

    serialport.println("pingrequest was sent");
}

// Sensor Node Mesh Callback for recieved messages. 
void sensornodeMessageRX(uint32_t from, String &recievedmsg) 
{
    Serial.println(recievedmsg);

    StaticJsonDocument<256> recieved;
    deserializeJson(recieved, recievedmsg);
    String type = recieved["type"];  

    if (type == "pingrequest") 
    {
        String pingID = recieved["data"]["ping"];
        respondPing(pingID);
    }
}

// Sensor Node Mesh Callback for new connections to the mesh
void sensornodeNewConnection(uint32_t nodeID) 
{
    StaticJsonDocument<256> message;

    message["type"] = "meshupdate";
    message["origin"] = mesh.getNodeId();

    message["reach"]["type"] = "internal";
    message["reach"]["set"] = "null";

    message["data"]["change"] = "newconnection";
    message["data"]["message"] = "New Node on Mesh";
    message["data"]["newnode"] = nodeID;

    String msg;
    serializeJson(message, msg);
    Serial.println(msg);
}

// Sensor Node Mesh Callback for modified connection messages
void sensornodeChangedConnection() 
{
    StaticJsonDocument<256> message;

    message["type"] = "meshupdate";
    message["origin"] = mesh.getNodeId();

    message["reach"]["type"] = "internal";
    message["reach"]["set"] = "null";

    message["data"]["change"] = "changedconnection";
    message["data"]["message"] = "Mesh Connections Modified";

    String msg;
    serializeJson(message, msg);
    Serial.println(msg);
}

// Sensor Node Mesh Callback for node time adjustment messages
void sensornodeNodeTimeAdjust(int32_t offset) 
{
    StaticJsonDocument<256> message;

    message["type"] = "meshupdate";
    message["origin"] = mesh.getNodeId();

    message["reach"]["type"] = "internal";
    message["reach"]["set"] = "null";

    message["data"]["change"] = "nodetimeadjust";
    message["data"]["message"] = "Node Time Adjusted";
    message["data"]["nodetime"] = mesh.getNodeTime();
    message["data"]["offset"] = offset;

    String msg;
    serializeJson(message, msg);
    Serial.println(msg);
}

// Control Node Mesh Callback for recieved messages. 
void controlnodeMessageRX(uint32_t from, String &recievedmsg) 
{
    serialport.println(recievedmsg);
}

// Control Node Mesh Callback for new connections to the mesh
void controlnodeNewConnection(uint32_t nodeID) 
{
    StaticJsonDocument<256> message;
    
    message["type"] = "meshupdate";
    message["origin"] = mesh.getNodeId();

    message["reach"]["type"] = "internal";
    message["reach"]["set"] = "null";

    message["data"]["change"] = "newconnection";
    message["data"]["message"] = "New Node on Mesh";
    message["data"]["newnode"] = nodeID;

    String msg;
    serializeJson(message, msg);
    serialport.println(msg);
}

// Control Node Mesh Callback for modified connection messages
void controlnodeChangedConnection() 
{
    StaticJsonDocument<256> message;
    
    message["type"] = "meshupdate";
    message["origin"] = mesh.getNodeId();

    message["reach"]["type"] = "internal";
    message["reach"]["set"] = "null";

    message["data"]["change"] = "changedconnection";
    message["data"]["message"] = "Mesh Connections Modified";

    String msg;
    serializeJson(message, msg);
    serialport.println(msg);
}

// Control Node Mesh Callback for node time adjustment messages
void controlnodeNodeTimeAdjust(int32_t offset) 
{
    StaticJsonDocument<256> message;
    
    message["type"] = "meshupdate";
    message["origin"] = mesh.getNodeId();

    message["reach"]["type"] = "internal";
    message["reach"]["set"] = "null";

    message["data"]["change"] = "nodetimeadjust";
    message["data"]["message"] = "Node Time Adjusted";
    message["data"]["nodetime"] = mesh.getNodeTime();
    message["data"]["offset"] = offset;

    String msg;
    serializeJson(message, msg);
    serialport.println(msg);
}

// FyrNode Constructor
FyrNode::FyrNode() {
    mesh.setDebugMsgTypes(ERROR|STARTUP);
    mesh.onReceive(&sensornodeMessageRX);
    mesh.onNewConnection(&sensornodeNewConnection);
    mesh.onChangedConnections(&sensornodeChangedConnection);
    mesh.onNodeTimeAdjusted(&sensornodeNodeTimeAdjust);
}

void FyrNode::begin() {
    Serial.begin(38400);

    if (DHTTYP > 0) {dht.begin();}
    if (GASTYP > 0) {pinMode(GASPIN, INPUT);}
    if (FLMTYP > 0) {pinMode(FLMPIN, INPUT);}

    mesh.init(MESH_SSID, MESH_PSWD, &meshScheduler, MESH_PORT);
    CONTROLNODE = 3160379558;
}

void FyrNode::update() {
    mesh.update();
}

//FyrNodeControl Constructor
FyrNodeControl::FyrNodeControl() {
    mesh.setDebugMsgTypes(ERROR|STARTUP);
    mesh.onReceive(&controlnodeMessageRX);
    mesh.onNewConnection(&controlnodeNewConnection);
    mesh.onChangedConnections(&controlnodeChangedConnection);
    mesh.onNodeTimeAdjusted(&controlnodeNodeTimeAdjust);
}

void FyrNodeControl::begin() {
    Serial.begin(38400);
    serialport.begin(38400);

    if (CONNECT == true) {connectbutton.begin();}
    if (PINGER == true) {pingerbutton.begin();}

    mesh.init(MESH_SSID, MESH_PSWD, &meshScheduler, MESH_PORT);
    CONTROLNODE = mesh.getNodeId();
}

void FyrNodeControl::update() {
    mesh.update();

    if (PINGER == true) {
        pingerbutton.read();
        if (pingerbutton.wasReleased()) {
            String pingID = String(random(0,9999));
            requestPing(pingID);
        }
    }
}