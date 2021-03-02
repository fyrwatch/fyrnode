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

uint32_t CONTROLNODE = 0;
bool MESHCONNECTED = false;
bool MESHHANDSHAKE = false;
int handshaketimer = 0;

painlessMesh mesh;
Scheduler meshScheduler;
DHT dht(DHTPIN, DHTTYP);
Button pingerbutton(PINGERPIN);
Button connectbutton(CONNECTPIN);

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

    Serial.println("pingrequest was sent");
}

// A function to respond to a 'handshakerequest' with a 'handshakeresponse'
void respondHandShake(uint32_t from) 
{
    StaticJsonDocument<256> message;
    
    message["type"] = "handshakeresponse";
    message["origin"] = mesh.getNodeId();

    message["reach"]["type"] = "unicast";
    message["reach"]["set"] = from;

    message["data"]["message"] = "handshake acknowledged";
    message["data"]["control"] = mesh.getNodeId();

    String msg;
    serializeJson(message, msg);
    mesh.sendSingle(from, msg);
}

// A function to broadcast a 'handshakerequest'
void requestHandShake()
{
    StaticJsonDocument<256> message;
    
    message["type"] = "handshakerequest";
    message["origin"] = mesh.getNodeId();

    message["reach"]["type"] = "broadcast";
    message["reach"]["set"] = "null";

    message["data"]["message"] = "handshake requested";

    String msg;
    serializeJson(message, msg);

    Serial.println("handshake request runtime intiated");
    Serial.println(msg);

    mesh.sendBroadcast(msg);
}

// Control Node Mesh Callback for recieved messages. 
void messageRXControl(uint32_t from, String &recievedmsg) 
{
    Serial.println(recievedmsg);

    StaticJsonDocument<256> recieved;
    deserializeJson(recieved, recievedmsg);
    String type = recieved["type"];  

    if (type == "handshakerequest") 
    {
        respondHandShake(from);
    }
}

// Node Mesh Callback for recieved messages. 
void messageRX(uint32_t from, String &recievedmsg) 
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
    else if (type == "handshakeresponse") 
    {
        uint32_t controlnode = recieved["data"]["control"];
        CONTROLNODE = controlnode;
    }
}

// Node Mesh Callback for new connections to the mesh
void newConnection(uint32_t nodeID) 
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

// Node Mesh Callback for modified connection messages
void changedConnection() 
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

// Node Mesh Callback for node time adjustment messages
void nodeTimeAdjust(int32_t offset) 
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

// A function to read incoming serial messages
void readIncoming()
{
    if (Serial.available())
    {
        StaticJsonDocument<300> doc;
        DeserializationError err = deserializeJson(doc, Serial);

        if (err == DeserializationError::Ok) {
            // Print the values
            // (we must use as<T>() to resolve the ambiguity)
            Serial.print("timestamp = ");
            Serial.println(doc["timestamp"].as<long>());
            Serial.print("value = ");
            Serial.println(doc["value"].as<String>());
        } 
        else {
            // Print error to the "debug" serial port
            Serial.print("deserializeJson() returned ");
            Serial.println(err.c_str());
        
            // Flush all bytes in the "link" serial port buffer
            while (Serial.available() > 0) {
                Serial.read();
            }
        }  
    }
    

    // if (serialbuffer > 0)
    // {
    //     String data;
    //     char bytedata;

    //     for (int i = 0; i < serialbuffer; i++)
    //     {
    //         bytedata = serialport.read();
    //         data = data+bytedata;
    //     }

    //     checkIncoming(data);
    // }    
}

// A function that checks the 'MESHCONNECTED'  
// variable and flips an LED accordingly.
void setConnectionLED() 
{
    if (MESHCONNECTED == true) {
        digitalWrite(16, LOW);
    } else {
        digitalWrite(16, HIGH);
    }
}

// A function to check if the node is connected to
// the control node and send a handshake otherwise.
void checkConnection() 
{
    if (CONTROLNODE > 0) {
        MESHCONNECTED = mesh.isConnected(CONTROLNODE);
    } 
    else if (handshaketimer < 10) {
        handshaketimer++;
    }
    else if (handshaketimer == 10) {
        requestHandShake();
        handshaketimer = 0;
    }
}

// A function that checks the pingerbutton for a press 
// and calls the 'requestPing' method.
void checkPINGER() {
    pingerbutton.read();
    if (pingerbutton.wasReleased()) {
        String pingID = String(random(0,9999));
        requestPing(pingID);
    }
}


// FyrNode Constructor
FyrNode::FyrNode() {
    mesh.setDebugMsgTypes(ERROR|STARTUP);
    mesh.onReceive(&messageRX);
    mesh.onNewConnection(&newConnection);
    mesh.onChangedConnections(&changedConnection);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjust);
}

void FyrNode::begin() {
    Serial.begin(38400);
    mesh.init(MESH_SSID, MESH_PSWD, &meshScheduler, MESH_PORT);

    if (DHTTYP > 0) {dht.begin();}
    if (GASTYP > 0) {pinMode(GASPIN, INPUT);}
    if (FLMTYP > 0) {pinMode(FLMPIN, INPUT);}
    pinMode(16, OUTPUT);
}

void FyrNode::update() {
    mesh.update();
    checkConnection();
    setConnectionLED();
    
}

//FyrNodeControl Constructor
FyrNodeControl::FyrNodeControl() {
    mesh.setDebugMsgTypes(ERROR|STARTUP);
    mesh.onReceive(&messageRXControl);
    mesh.onNewConnection(&newConnection);
    mesh.onChangedConnections(&changedConnection);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjust);
}

void FyrNodeControl::begin() {
    Serial.begin(38400);

    if (CONNECT == true) {connectbutton.begin();}
    if (PINGER == true) {pingerbutton.begin();}

    mesh.init(MESH_SSID, MESH_PSWD, &meshScheduler, MESH_PORT);
    CONTROLNODE = mesh.getNodeId();
}

void FyrNodeControl::update() {
    mesh.update();

    readIncoming();
    if (PINGER == true) {checkPINGER();}
}