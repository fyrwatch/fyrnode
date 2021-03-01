// ===========================================================================
// Copyright (C) 2020 Manish Meganathan, Mariyam A.Ghani. All Rights Reserved.
// 
// This file is part of the FyrNode library.
// No part of the FyrNode library can not be copied and/or distributed 
// without the express permission of Manish Meganathan and Mariyam A.Ghani
// ===========================================================================
// Code for control node to forward all recived messages to its software serial 
// port and to send ping request  to the mesh when a button is pressed. 
//
// Serial Baud Rate - 38400 (HW & SW Serial)
// Software Serial Pins - D5, D6
// Button is attached to D0 (PIN16)
//
// Written for NodeMCU 1.0 ESP-12E board
// ===========================================================================

#include "painlessMesh.h"
#include "SoftwareSerial.h"
#include "ArduinoJson.h"
#include "JC_Button.h"

#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

#define RXPIN D5
#define TXPIN D6
#define BUTTON_PIN 16

painlessMesh mesh;
Scheduler userScheduler;
Button button(BUTTON_PIN);
SoftwareSerial serialport(RXPIN, TXPIN);


//painlessMesh Callback for recieved messages
void receivedCallback(uint32_t from, String &recievedmsg) 
{
    serialport.println(recievedmsg);
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
    serialport.println(msg);
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
    serialport.println(msg);
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
    serialport.println(msg);
}

void printConfig() 
{
    StaticJsonDocument<256> message;
    String msg;

    message["type"] = "configresponse";
    message["origin"] = mesh.getNodeId();
    message["reach"] = "internal-0";

    message["data"]["nodeID"] = mesh.getNodeId();

    serializeJson(message, msg);
    serialport.println(msg);
}

void sendPingRequest(String &ping)
{
    StaticJsonDocument<256> message; 
    String msg;

    message["type"] = "pingrequest";
    message["origin"] = mesh.getNodeId();
    message["reach"] = "broadcast";

    message["data"]["ping"] = ping;

    serializeJson(message, msg);
    mesh.sendBroadcast(msg);

    serialport.println("pingrequest was sent");
}

void checkIncoming(String &data)
{
    StaticJsonDocument<256> message;
    deserializeJson(message, data);
    String type = message['type'];

    if (type == "pingrequest")
    {
        String pingID = message['data']['ping'];
        sendPingRequest(pingID);
    }
    else if (type == "configrequest")
    {
        printConfig();
    }
}

void readIncoming()
{
    int serialbuffer = serialport.available();

    if (serialbuffer > 0)
    {
        String data;
        char bytedata;

        for (int i = 0; i < serialbuffer; i++)
        {
            bytedata = serialport.read();
            data = data+bytedata;
        }

        checkIncoming(data);
    }    
}

void setup()
{
    Serial.begin(38400);
	serialport.begin(38400);
    
    mesh.setDebugMsgTypes( ERROR | STARTUP );
    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);

    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    printConfig();
}

void loop()
{
	mesh.update();
    readIncoming();

    button.read();
    if (button.wasReleased()) 
    {
        String pingID = String(random(0,9999));
        sendPingRequest(pingID);
    }
}
