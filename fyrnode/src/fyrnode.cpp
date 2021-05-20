// ===========================================================================
// Copyright (C) 2020 Manish Meganathan, Mariyam A.Ghani. All Rights Reserved.
// 
// This file is part of the FyrNode library.
// No part of the FyrNode library can not be copied and/or distributed 
// without the express permission of Manish Meganathan and Mariyam A.Ghani
// ===========================================================================

// Dependancies
#include "fyrnode.h"
#include "Arduino.h"
#include "painlessMesh.h"
#include "ArduinoJson.h"
#include "JC_Button.h"
#include "DHT.h"
#include "String"

// Mesh AP Configuration Values
extern String MESH_SSID;
extern String MESH_PSWD;
extern uint16_t MESH_PORT;
// Node Sensor Hardware Configuration Values
extern int DHTTYP;
extern int DHTPIN;
extern int GASTYP;
extern int GASPIN;
extern int FLMTYP;
extern int FLMPIN;
// Node Button Hardware Configuration Values
extern bool PINGER;
extern int PINGERPIN;
// Node Serial Interface Configuration Value
extern uint32_t SERIALBAUD;
// Node Connection LED Configuration Value
extern int CONNECTLEDPIN;

// Global Runtime Variables
uint32_t handshaketimer = 0;
uint32_t MESHCONTROLNODE = 0;
bool MESHCONNECTED = false;

// Global Runtime Objects
painlessMesh mesh;
Scheduler meshScheduler;
DHT dht(DHTPIN, DHTTYP);
Button pingerButton(PINGERPIN);


/*
A function that sends messages to the mesh based on the 'reach' parameters of the message document passed.

Unicast - message is sent to node set in 'destination'
Broadcast - message is sent to all nodes on the mesh.

Refer to API documentation for more information on the reach parameters of mesh messages.
*/
void sendmeshmessage(DynamicJsonDocument &messagedoc) 
{
    // Serialize the Document into a String
    String message; serializeJson(messagedoc, message);
    // Detect the transmit method from the reach parameters of the document
    String transmitmethod = messagedoc["reach"]["type"];

    // Check the transmit method and perform the appropriate runtime
    if (transmitmethod == "unicast") {
        // Detect the destination from the reach parameters of the document
        uint32_t destination = messagedoc["reach"]["destination"].as<uint32_t>();
        // Unicast Transmit to the destination node
        mesh.sendSingle(destination, message);
    }
    else if (transmitmethod == "broadcast") {
        // Broadcast Transmit to all nodes
        mesh.sendBroadcast(message);
    }
}


// A function that reads the DHT sensor value and fills it into the passed document.
void readsensor_DHT(DynamicJsonDocument &message) 
{
    // Read Humidity and Temperature values from the sensor.
    float hum = dht.readHumidity();
    float tem = dht.readTemperature();
    // Fill Humidity and Temperature values into the document.
    message["data"]["sensors"]["HUM"] = hum;
    message["data"]["sensors"]["TEM"] = tem;
}


// A function that reads the GAS sensor value and fills it into the passed document.
void readsensor_GAS(DynamicJsonDocument &message) 
{
    // Read Analog value from the sensor.
    int g = analogRead(GASPIN);
    // Fill value into the document
    message["data"]["sensors"]["GAS"] = g;
}


// A function that reads the FLM sensor value and fills it into the passed document.
void readsensor_FLM(DynamicJsonDocument &message) 
{
    // Read Digital value from the sensor.
    int f = digitalRead(FLMPIN);
    // Fill value into the document
    message["data"]["sensors"]["FLM"] = f;
}


/*
A command handler that responds to the command 'readsensors'. 
The runtime fills in the sensor readings based on the hardware configuration values, 
wraps it into a 'sensordata' message and sends it to the MESHCONTROLNODE.
*/
void handlecommand_readsensors(String pingid) 
{
    // Create the sensordata document
    DynamicJsonDocument sensordata(1024);
    sensordata["type"] = "message";
    sensordata["origin"] = mesh.getNodeId();
    // Set the reach parameters to unicast with the Control Node as the destination
    sensordata["reach"]["type"] = "unicast";
    sensordata["reach"]["destination"] = MESHCONTROLNODE;
    // Fill in the ping ID and set the message type.
    sensordata["data"]["ping"] = pingid;
    sensordata["data"]["type"] = "sensordata";

    // Fill in sensordata readings for DHT
    if (DHTTYP > 0) {readsensor_DHT(sensordata);}
    // Fill in sensordata readings for GAS
    if (GASTYP > 0) {readsensor_GAS(sensordata);}
    // Fill in sensordata readings for FLM
    if (FLMTYP > 0) {readsensor_FLM(sensordata);}

    // Transmit the sensordata
    sendmeshmessage(sensordata);
}


/*
A command handler that responds to the command 'readconfig'.
The runtime fills in the configuration values from the hardware configuration values,
wraps it into a 'configdata' message and sends it to the MESHCONTROLNODE.
*/
void handlecommand_readconfig(String pingid) 
{
    // Create the sensordata document
    DynamicJsonDocument configdata(1024);
    configdata["type"] = "message";
    configdata["origin"] = mesh.getNodeId();
    // Set the reach parameters to unicast with the Control Node as the destination
    configdata["reach"]["type"] = "unicast";
    configdata["reach"]["destination"] = MESHCONTROLNODE;
    // Fill in the ping ID and set the message type.
    configdata["data"]["ping"] = pingid;
    configdata["data"]["type"] = "configdata";

    // Fill in the node id
    configdata["data"]["config"]["NODEID"] = mesh.getNodeId();
    // Fill in the sensor configuration values
    configdata["data"]["config"]["DHTTYP"] = DHTTYP;
    configdata["data"]["config"]["DHTPIN"] = DHTPIN;
    configdata["data"]["config"]["GASTYP"] = GASTYP;
    configdata["data"]["config"]["GASPIN"] = GASPIN;
    configdata["data"]["config"]["FLMTYP"] = FLMTYP;
    configdata["data"]["config"]["FLMPIN"] = FLMPIN;
    // Fill in the other hardware configuration values
    configdata["data"]["config"]["PINGER"] = PINGER;
    configdata["data"]["config"]["PINGERPIN"] = PINGERPIN;
    configdata["data"]["config"]["SERIALBAUD"] = SERIALBAUD;
    configdata["data"]["config"]["CONNECTLEDPIN"] = CONNECTLEDPIN;

    // Transmit the sensordata
    sendmeshmessage(configdata);
}


/*
A control command handler that responds to the control command 'readconfig-control'.
Accumulates the relevant configuration values for the hardware and mesh into a 
meshlog of type 'controlconfigdata' and logs it to the Serial.
*/
void handlecontrolcommand_readconfig() 
{
    // Create the meshlog document
    StaticJsonDocument<1024> logdoc;
    logdoc["type"] = "meshlog";
    logdoc["nodeID"] = mesh.getNodeId();
    logdoc["nodetime"] = mesh.getNodeTime();
    // Fill in the meshlog values
    logdoc["logdata"]["type"] = "controlconfigdata";
    logdoc["logdata"]["message"] = "control config data received";
    logdoc["logdata"]["node"] = mesh.getNodeId();

    // Fill in the hardware configuration values
    logdoc["logdata"]["config"]["PINGER"] = PINGER;
    logdoc["logdata"]["config"]["PINGERPIN"] = PINGERPIN;
    logdoc["logdata"]["config"]["SERIALBAUD"] = SERIALBAUD;
    logdoc["logdata"]["config"]["CONNECTLEDPIN"] = CONNECTLEDPIN;
    // Fill in the mesh configuration values
    logdoc["logdata"]["config"]["MESH_SSID"] = MESH_SSID;
    logdoc["logdata"]["config"]["MESH_PSWD"] = MESH_PSWD;
    logdoc["logdata"]["config"]["MESH_PORT"] = MESH_PORT;
    logdoc["logdata"]["config"]["NODEID"] = mesh.getNodeId();

    // Log the document to the Serial port.
    serializeJson(logdoc, Serial); Serial.println();
}

/*
A control command handler that responds to the control command 'readnodelist'.
Accumulates the list of nodes connected to the mesh into a 
meshlog of type 'controlnodelist' and logs it to the Serial.
*/
void handlecontrolcommand_nodelist() 
{
    // Create an empty string
    String strnodelist = "";
    // Retrieve the list of connected nodes from the mesh
    std::list<uint32_t> nodelist = mesh.getNodeList();
    // Accumulate the nodelist into a '-' separated string
    for (uint32_t i : nodelist) {strnodelist = strnodelist + i + "-";}

    // Create the meshlog document. A DynamicJSONDocument is used
    // to avoid space limitations as the cluster size increases.
    DynamicJsonDocument logdoc(1024);
    logdoc["type"] = "meshlog";
    logdoc["nodeID"] = mesh.getNodeId();
    logdoc["nodetime"] = mesh.getNodeTime();
    // Fill in the meshlog valuesre
    logdoc["logdata"]["type"] = "controlnodelist";
    logdoc["logdata"]["message"] = "control nodelist data received";
    logdoc["logdata"]["node"] = mesh.getNodeId();

    // Attach the nodelist to the meshlog
    logdoc["logdata"]["nodelist"] = strnodelist;

    // Log the document to the Serial port.
    serializeJson(logdoc, Serial); Serial.println();
}


/*
A message handler triggered when a 'meshcommand' message is received by the node. 
Calls the appropriate 'handlecommand_' runtime to execute the command instruction. 
*/
void handlemessage_meshcommand(DynamicJsonDocument commandmessage) 
{
    // Validate the message type to be a 'meshcommand'
    if (commandmessage["data"]["type"] == "meshcommand") {
        // Determine the command.
        String command = commandmessage["data"]["command"];

        // Create the meshlog document
        StaticJsonDocument<512> logdoc;
        logdoc["type"] = "meshlog";
        logdoc["nodeID"] = mesh.getNodeId();
        logdoc["nodetime"] = mesh.getNodeTime();
        // Fill in the meshlog values
        logdoc["logdata"]["type"] = "meshcommandreceived";
        logdoc["logdata"]["message"] = "command received from the mesh";
        logdoc["logdata"]["command"] = command;
        // Log the document to the Serial port.
        serializeJson(logdoc, Serial); Serial.println();

        // Call the appropriate command handler runtime.
        if (command == "readsensors") {
            String pingid = commandmessage["data"]["ping"];
            handlecommand_readsensors(pingid);
        }
        else if (command == "readconfig") {
            String pingid = commandmessage["data"]["ping"];
            handlecommand_readconfig(pingid);
        }
    }
}


/*
A message handler triggered when a 'handshake' message is received by the node. 
Sends a 'handshakeACK' message back to the node that sent the 'handshake' message and sends a 
'readconfig' command to the same node and finally logs the 'handshakerequested' meshlog to the Serial port
*/
void handlemessage_handshake(DynamicJsonDocument handshakemessage) 
{
    // Validate the message type to be a 'handshake'
    if (handshakemessage["data"]["type"] == "handshake") {
        // Determine the nodeID requesting a handshake
        uint32_t friendlynode = handshakemessage["origin"].as<uint32_t>();

        // Create the handshakeACK document
        DynamicJsonDocument handshakeACK(512);
        handshakeACK["type"] = "message";
        handshakeACK["origin"] = mesh.getNodeId();
        // Fill in the reach parameters
        handshakeACK["reach"]["type"] = "unicast";
        handshakeACK["reach"]["destination"] = friendlynode;
        // Fill in the handshakeACK values
        handshakeACK["data"]["type"] = "handshakeACK";
        handshakeACK["data"]["controlnode"] = mesh.getNodeId();
        handshakeACK["data"]["message"] = "handshake acknowledged";

        // Transmit the handshakeACK
        sendmeshmessage(handshakeACK);

        //TODO: configdata request code would come here.

        // Create the meshlog document
        StaticJsonDocument<512> logdoc;
        logdoc["type"] = "meshlog";
        logdoc["nodeID"] = mesh.getNodeId();
        logdoc["nodetime"] = mesh.getNodeTime();
        // Fill in the meshlog values
        logdoc["logdata"]["type"] = "handshake-rxack";
        logdoc["logdata"]["message"] = "handshake requested and acknowledged for a node on the mesh";
        logdoc["logdata"]["node"] = friendlynode;
        // Log the document to the Serial port.
        serializeJson(logdoc, Serial); Serial.println();
    }
}


/*
A message handler triggered when a 'handshakeACK' message is received by the node. 
Sets the MESHCONTROLNODE global, logs a 'handshakecomplete' meshlog to the Serial.
*/
void handlemessage_handshakeACK(DynamicJsonDocument handshakemessage) 
{
    // Validate the message type to be a 'handshakeACK'
    if (handshakemessage["data"]["type"] == "handshakeACK") {
        // Determine the ControlNodeID from the acknowledgement
        MESHCONTROLNODE = handshakemessage["data"]["controlnode"].as<uint32_t>();

        // Create the meshlog document
        StaticJsonDocument<512> logdoc;
        logdoc["type"] = "meshlog";
        logdoc["nodeID"] = mesh.getNodeId();
        logdoc["nodetime"] = mesh.getNodeTime();
        // Fill in the meshlog values
        logdoc["logdata"]["type"] = "handshakecomplete";
        logdoc["logdata"]["message"] = "handshake completed with control node";
        logdoc["logdata"]["controlnode"] = MESHCONTROLNODE;
        // Log the document to the Serial port.
        serializeJson(logdoc, Serial); Serial.println();
    }    
}


/*
A message handler triggered when a 'sensordata' message is received by the node.
Reads the message and logs a meshlog of type 'sensordata' to the Serial.
*/
void handlemessage_sensordata(DynamicJsonDocument sensordata)
{
    // Validate the message type to be a 'sensordata'
    if (sensordata["data"]["type"] == "sensordata") {
        uint32_t nodeID = sensordata["origin"].as<uint32_t>();
        String pingid = sensordata["data"]["ping"];

        // Create the meshlog document
        StaticJsonDocument<1024> logdoc;
        logdoc["type"] = "meshlog";
        logdoc["nodeID"] = mesh.getNodeId();
        logdoc["nodetime"] = mesh.getNodeTime();
        // Fill in the meshlog values
        logdoc["logdata"]["type"] = "sensordata";
        logdoc["logdata"]["message"] = "sensor data received";
        logdoc["logdata"]["node"] = nodeID;
        logdoc["logdata"]["ping"] = pingid;
        logdoc["logdata"]["sensors"] = sensordata["data"]["sensors"];
        // Log the document to the Serial port.
        serializeJson(logdoc, Serial); Serial.println();
    }
}

/*
A message handler triggered when a 'configdata' message is recieved by the node.
Reads the message and logs a meshlog of type 'configdata' to the Serial.
*/
void handlemessage_configdata(DynamicJsonDocument configdata)
{
    // Validate the message type to be a 'configdata'
    if (configdata["data"]["type"] == "configdata") {
        uint32_t nodeID = configdata["origin"].as<uint32_t>();
        String pingid = configdata["data"]["ping"];

        // Create the meshlog document
        StaticJsonDocument<1024> logdoc;
        logdoc["type"] = "meshlog";
        logdoc["nodeID"] = mesh.getNodeId();
        logdoc["nodetime"] = mesh.getNodeTime();
        // Fill in the meshlog values
        logdoc["logdata"]["type"] = "configdata";
        logdoc["logdata"]["message"] = "config data received";
        logdoc["logdata"]["node"] = nodeID;
        logdoc["logdata"]["ping"] = pingid;
        logdoc["logdata"]["config"] = configdata["data"]["config"];
        // Log the document to the Serial port.
        serializeJson(logdoc, Serial); Serial.println();
    }
}


/*
A command sender for the 'readsensors' command. 

If node argument passed is 0, the command is sent in broadcast mode i.e to all the nodes. 
Otherwise, it is sent only to nodeID that is passed in unicast mode.

If the pingid argument is "control", the command will generate a new pingid in the format 'controlping-<random 6 digit number>' and similarly,
If the pingid argument is "remote", the command will generate a new pingid in the format 'remoteping-<random 6 digit number>'.
For all other value of pingid, it is used as it for the consequent ping command.
*/
void sendcommand_readsensors(uint32_t node, String pingid) 
{
    // Create command document
    DynamicJsonDocument requestsensordata(512); 
    requestsensordata["type"] = "message";
    requestsensordata["origin"] = mesh.getNodeId();
    // Fill in the reach parameters
    if (node == 0) {
        // If value of node is 0, set the reach to 'broadcast'
        requestsensordata["reach"]["type"] = "broadcast";
    } else {
        // If value of node is passed, set it as the destination for a 'unicast' reach
        requestsensordata["reach"]["type"] = "unicast";
        requestsensordata["reach"]["destination"] = node;
    }
    // Fill in the command values and metadata
    requestsensordata["data"]["type"] = "meshcommand";
    requestsensordata["data"]["command"] = "readsensors";
    requestsensordata["data"]["message"] = "sensor data requested";
    requestsensordata["data"]["ping"] = pingid;

    // Transmit the command
    sendmeshmessage(requestsensordata);
} 


/*
A command sender for the 'readconfig' command.

If node argument passed is 0, the command is sent in broadcast mode i.e to all the nodes. 
Otherwise, it is sent only to nodeID that is passed in unicast mode.

If the pingid argument is "control", the command will generate a new pingid in the format 'controlping-<random 6 digit number>' and similarly,
If the pingid argument is "remote", the command will generate a new pingid in the format 'remoteping-<random 6 digit number>'.
For all other value of pingid, it is used as it for the consequent ping command.
*/
void sendcommand_readconfig(uint32_t node, String pingid)
{
    // Check if a pingid needs to be generated
    if (pingid == "control") {
        // Generate a random ping ID for control node pings.
        pingid = "controlping" + String(random(100000,999999));
    } else if (pingid == "remote") {
        // Generate a randome ping ID for remote node pings.
        pingid = "remoteping" + String(random(100000,999999));
    }

    DynamicJsonDocument requestconfigdata(512); 
    requestconfigdata["type"] = "message";
    requestconfigdata["origin"] = mesh.getNodeId();

    if (node == 0) {
        // If value of node is 0, set the reach to 'broadcast'
        requestconfigdata["reach"]["type"] = "broadcast";
    } else {
        // If value of node is passed, set it as the destination for a 'unicast' reach
        requestconfigdata["reach"]["type"] = "unicast";
        requestconfigdata["reach"]["destination"] = node;
    }

    // Fill in the command values and metadata
    requestconfigdata["data"]["type"] = "meshcommand";
    requestconfigdata["data"]["command"] = "readconfig";
    requestconfigdata["data"]["message"] = "config data requested";
    requestconfigdata["data"]["ping"] = pingid;

    sendmeshmessage(requestconfigdata);
}   


/*
A function that handles commands received from the controller on the Serial port. 
Checks the command and calls the appropriate 'sendcommand_' method
*/
void handlecontrolcommand(DynamicJsonDocument controlcommand)
{
    // Determine the command from the controlmessage
    String command = controlcommand["command"];

    // Check the command and call the appropriate runtime.
    if (command == "connection-on") {
        MESHCONNECTED = true;
    }
    else if (command == "connection-off") {
        MESHCONNECTED = false;
    }
    else if (command == "readsensors-mesh") {
        // Detect the ping ID
        String pingid = controlcommand["ping"].as<String>();
        // Send the 'readsensor' command in broadcast mode
        sendcommand_readsensors(0, pingid);
    }
    else if (command == "readsensors-node") {
        // Detect the destination node and ping ID
        uint32_t node = controlcommand["node"].as<uint32_t>();
        String pingid = controlcommand["ping"].as<String>();
        // Send the 'readsensor' command in unicast mode
        sendcommand_readsensors(node, pingid);
    }
    else if (command == "readconfig-mesh") {
        // Detect the ping ID
        String pingid = controlcommand["ping"].as<String>();
        // Send the 'readconfig' command
        sendcommand_readconfig(0, pingid);
    }
    else if (command == "readconfig-node") {
        // Detect the destination node and ping ID
        uint32_t node = controlcommand["node"].as<uint32_t>();
        String pingid = controlcommand["ping"].as<String>();
        // Send the 'readconfig' command
        sendcommand_readconfig(0, pingid);
    }
    else if (command == "readconfig-control") {
        handlecontrolcommand_readconfig();
    }
    else if (command == "readnodelist-control") {
        handlecontrolcommand_nodelist();
    }
}


/* 
A Mesh callback function triggered when there is a new connections to the mesh. 
This callback is used by both the FyrNode and FyrNodeControl objects. 
The callback generates a 'meshlog' document of type 'newconnection' and logs it to the Serial.
Refer to the API documentation for more information about the 'meshlog' structure.
*/
void meshcallback_newconnection(uint32_t nodeID) 
{   
    // Create the meshlog document
    StaticJsonDocument<512> logdoc;
    logdoc["type"] = "meshlog";
    logdoc["nodeID"] = mesh.getNodeId();
    logdoc["nodetime"] = mesh.getNodeTime();
    // Fill in the meshlog values
    logdoc["logdata"]["type"] = "newconnection";
    logdoc["logdata"]["message"] = "New Node added on Mesh";
    logdoc["logdata"]["newnode"] = nodeID;
    // Log the document to the Serial port.
    serializeJson(logdoc, Serial); Serial.println();
}


/* 
A Mesh callback function triggered when there is a change to the connections of the mesh. 
This callback is used by both the FyrNode and FyrNodeControl objects. 
The callback generates a 'meshlog' document of type 'changedconnection' and logs it to the Serial.
Refer to the API documentation for more information about the 'meshlog' structure.
*/
void meshcallback_changedconnection() 
{
    // Create the meshlog document
    StaticJsonDocument<512> logdoc;
    logdoc["type"] = "meshlog";
    logdoc["nodeID"] = mesh.getNodeId();
    logdoc["nodetime"] = mesh.getNodeTime();
    // Fill in the meshlog values
    logdoc["logdata"]["type"] = "changedconnection";
    logdoc["logdata"]["message"] = "Mesh Connections Modified";
    // Log the document to the Serial port.
    serializeJson(logdoc, Serial); Serial.println();
}


/* 
A Mesh callback function triggered when there is an adjustment to the node's mesh time. 
This callback is used by both the FyrNode and FyrNodeControl objects. 
The callback generates a 'meshlog' document of type 'nodetimeadjust' and logs it to the Serial.
Refer to the API documentation for more information about the 'meshlog' structure.
*/
void meshcallback_nodetimeadjust(int32_t offset) 
{
    // Create the meshlog document
    StaticJsonDocument<512> logdoc;
    logdoc["type"] = "meshlog";
    logdoc["nodeID"] = mesh.getNodeId();
    logdoc["nodetime"] = mesh.getNodeTime();
    // Fill in the meshlog values
    logdoc["logdata"]["type"] = "nodetimeadjust";
    logdoc["logdata"]["message"] = "Node Time Adjusted";
    logdoc["logdata"]["nodetime"] = mesh.getNodeTime();
    logdoc["logdata"]["offset"] = offset;
    // Log the document to the Serial port.
    serializeJson(logdoc, Serial); Serial.println();
}


/* 
A Mesh callback function triggered when a message has been received by the node. 
This callback is exclusively used by FyrNode objects. 
The callback prints the received message to the Serial for logging and deserializes it into 
a JSON document and calls the appropriate 'handlemessage_' runtime to handle the received message. 
Refer to the API documentation for more information about message handler runtimes.
*/
void meshcallback_messagerx(uint32_t from, String &receivedmessage) 
{
    // Create a document and deserialize the received message
    DynamicJsonDocument message(512);
    deserializeJson(message, receivedmessage);
    // Detect the type of the received message
    String messagetype = message["data"]["type"];

    // Check the messagetype and call the appropriate runtime
    if (messagetype == "meshcommand") {
        // Call the 'meshcommand' message handler
        handlemessage_meshcommand(message);
    } 
    else if (messagetype == "handshakeACK") {
        // Call the 'handshakeACK' message handler
        handlemessage_handshakeACK(message);
    }
    else {
        // Create the meshlog document for the message of unknown type
        StaticJsonDocument<512> logdoc;
        logdoc["type"] = "meshlog";
        logdoc["nodeID"] = mesh.getNodeId();
        logdoc["nodetime"] = mesh.getNodeTime();
        // Fill in the meshlog values
        logdoc["logdata"]["type"] = "messagerx";
        logdoc["logdata"]["message"] = "Message Received";
        logdoc["logdata"]["rxtype"] = messagetype;
        // Log the document to the Serial port.
        serializeJson(logdoc, Serial); Serial.println();
    }
}


/* 
A Mesh callback function triggered when a message has been received by a control node. 
This callback is exclusively used by FyrNodeControl objects. 
The callback prints the received message to the Serial for logging and deserializes it into 
a JSON document and calls the appropriate 'handlemessage_' runtime to handle the received message. 
Refer to the API documentation for more information about message handler runtimes.
*/
void meshcallback_controlnode_messagerx(uint32_t from, String &receivedmessage) 
{
    // Create a document and deserialize the received message
    DynamicJsonDocument message(512);
    deserializeJson(message, receivedmessage);
    // Detect the type of the received message
    String messagetype = message["data"]["type"];

    // Check the messagetype and call the appropriate runtime
    if (messagetype == "handshake") {
        // Call the 'handshake' message handler
        handlemessage_handshake(message);
    } 
    else if (messagetype == "sensordata") {
        // Call the 'sensordata' message handler
        handlemessage_sensordata(message);
    }
    else if (messagetype == "configdata") {
        // Call the 'configdata' message handler
        handlemessage_configdata(message);
    }
    else {
        // Create the meshlog document for the message of unknowntype
        StaticJsonDocument<512> logdoc;
        logdoc["type"] = "meshlog";
        logdoc["nodeID"] = mesh.getNodeId();
        logdoc["nodetime"] = mesh.getNodeTime();
        // Fill in the meshlog values
        logdoc["logdata"]["type"] = "messagerx";
        logdoc["logdata"]["message"] = "Message Received";
        logdoc["logdata"]["rxtype"] = messagetype;
        // Log the document to the Serial port.
        serializeJson(logdoc, Serial); Serial.println();
    }
}


/*
A button check runtime that reads the button attached to PINGERPIN on any node.
Sends the 'readsensors' command if the button has been pressed.
*/
void checkbutton_pinger() {
    // Read the button status
    pingerButton.read();

    // On button press
    if (pingerButton.wasReleased()) {
        // Send the 'readsensor' command
        String pingid = "buttonping-" + String(random(100000,999999));
        sendcommand_readsensors(0, pingid);
    }
}


/*
A function that runs the handshake runtime. 
Checks the handshaketimer's value and increments it if its under the limit. 
If the limit is exceeded, a message of type 'handshake' is generated and broadcast to the whole mesh. 
The handshaketimer is also reset everytime the message is broadcast.
Refer to the API documentation for more information about the handshake runtime.
*/
void runhandshake()
{
    // Check if the handshaketimer has run out
    if (handshaketimer < 100000) {
        // Increment the timer
        handshaketimer++;
    } else {
        // Create a handshake message document
        DynamicJsonDocument handshake(512);
        handshake["type"] = "message";
        handshake["origin"] = mesh.getNodeId();
        // Set the reach parameters to broadcast
        handshake["reach"]["type"] = "broadcast";
        // Set the message type and message
        handshake["data"]["type"] = "handshake";
        handshake["data"]["message"] = "handshake requested";
        // Reset the handshaketimer
        handshaketimer = 0;
        // Transmit the handshake
        sendmeshmessage(handshake);
    }
}


// A function that check the MESHCONNECTED global flag and sets the LED on the CONNECTLEDPIN
void setconnectionLED()
{
    // Check if the MESHCONNECTED value is True
    if (MESHCONNECTED == true) {
        // Set the LED to ON
        digitalWrite(CONNECTLEDPIN, LOW);
    } else {
        // Set the LED to OFF
        digitalWrite(CONNECTLEDPIN, HIGH);
    }
}


/*
A function that checks if the node is currently connected to 'larger' mesh.
This is achieved by first checking if the MESHCONTROLNODE global has been set.

If it has not been set, the handshake runtime is called.
If it has been set, the mesh control node can be verified to be connected to the mesh object.
*/
void checkmeshconnection()
{
    // Check if the MESHCONTROLNODE value has been acquired
    if (MESHCONTROLNODE > 0) {
        // Set the connection bool
        MESHCONNECTED = mesh.isConnected(MESHCONTROLNODE);
    } else {
        // Run the handshake runtime to acquire the MESHCONTROLNODE value
        runhandshake();
    }
}


/*
A function that checks the Serial port for messages from the controller.
The message is deserialized, and the type is checked and the appropriate handler is called. 
If the deserialization fails, the Serial buffer is flushed.
*/
void checkcontrollermessages()
{
    // Check if there is data in the Serial buffer
    if (Serial.available()) {
        // Create message document
        DynamicJsonDocument message(512);
        // Deserialize the message from the Serial buffer
        DeserializationError error = deserializeJson(message, Serial);

        // Check for Deserialization Error
        if (error == DeserializationError::Ok) {
            // Handle valid 'controlcommand' messages
            if (message["type"] == "controlcommand") {
                handlecontrolcommand(message);
            }
        } 
        else { 
            // Flush Serial buffer until it is empty
            while (Serial.available() > 0) {
                Serial.read();
            } 
        } 
    }
}


// FyrNode Object Constructor
FyrNode::FyrNode() 
{
    // Set the Mesh Debug Mode
    mesh.setDebugMsgTypes(ERROR|STARTUP);
    // Set the Mesh Callbacks
    mesh.onReceive(&meshcallback_messagerx);
    mesh.onNewConnection(&meshcallback_newconnection);
    mesh.onChangedConnections(&meshcallback_changedconnection);
    mesh.onNodeTimeAdjusted(&meshcallback_nodetimeadjust);
}

// FyrNode Object Initialisation Method
void FyrNode::begin() 
{
    // Initialise the Serial Port
    Serial.begin(SERIALBAUD);
    // Initialise the Mesh AP
    mesh.init(MESH_SSID, MESH_PSWD, &meshScheduler, MESH_PORT);
    // Set the Connection LED Pin to Output
    pinMode(CONNECTLEDPIN, OUTPUT);
    // Initialise Sensor objects and set pins to Input
    if (DHTTYP > 0) {dht.begin();}
    if (GASTYP > 0) {pinMode(GASPIN, INPUT);}
    if (FLMTYP > 0) {pinMode(FLMPIN, INPUT);}
    // Initialise the Button objects
    if (PINGER == true) {pingerButton.begin();}
}

// FyrNode Object Loop Method
void FyrNode::update() 
{
    // Update the Mesh Object
    mesh.update();
    // Check the Mesh Connection Status
    checkmeshconnection();
    // Set the connection LED
    setconnectionLED();
    // Check Pinger Button
    if (PINGER == true) {checkbutton_pinger();}
}


//FyrNodeControl Constructor
FyrNodeControl::FyrNodeControl() 
{
    // Set the Mesh Debug Mode
    mesh.setDebugMsgTypes(ERROR|STARTUP);
    // Set the Mesh Callbacks
    mesh.onReceive(&meshcallback_controlnode_messagerx);
    mesh.onNewConnection(&meshcallback_newconnection);
    mesh.onChangedConnections(&meshcallback_changedconnection);
    mesh.onNodeTimeAdjusted(&meshcallback_nodetimeadjust);
}

// FyrNodeControl Object Initialisation Method
void FyrNodeControl::begin() 
{
    // Initialise the Serial Port
    Serial.begin(SERIALBAUD);
    // Initialise the Mesh AP
    mesh.init(MESH_SSID, MESH_PSWD, &meshScheduler, MESH_PORT);
    // Set the Connection LED Pin to Output
    pinMode(CONNECTLEDPIN, OUTPUT);
    // Set Mesh Variables
    MESHCONTROLNODE = mesh.getNodeId();
    // Initialise the Button objects
    if (PINGER == true) {pingerButton.begin();}
}

// FyrNodeControl Object Loop Method
void FyrNodeControl::update() 
{
    // Update the Mesh Object
    mesh.update();
    // Check for messages from controller
    checkcontrollermessages();
    // Set the connection LED
    setconnectionLED();
    // Check Pinger Button
    if (PINGER == true) {checkbutton_pinger();}
}
