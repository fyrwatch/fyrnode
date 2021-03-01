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
// Flame Dout pin attached to D1.
// Gas Aout pin attached to A0
// Baud Rate - 115200
//
// Written for NodeMCU 1.0 ESP-12E board
// ===========================================================================

#include "painlessMesh.h"
#include "ArduinoJson.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

#define   GAS A0
#define   FLAME D1

painlessMesh  mesh;
Scheduler userScheduler;

void respondPing(uint32_t origin, String &pingID) {
  DynamicJsonDocument doc(1024);
  doc["pingID"] = pingID;

  int g = analogRead(GAS);
  int f = digitalRead(FLAME);
  
  doc["GAS"] = g;
  doc["FLAME"] = f;
  doc["NodeID"] = mesh.getNodeId();
  
  String msg; serializeJson(doc, msg);
  mesh.sendSingle(origin, msg);
}

void checkMessage(String &msg, uint32_t origin) {
  DynamicJsonDocument recieved(1024);
  deserializeJson(recieved, msg);

  String pingID = recieved["pingID"];  
  if (pingID != "null"){
    Serial.printf("ping request detected from %u with identifier %s\n", origin, pingID.c_str());
    respondPing(origin, pingID);
  }
}

// Needed for painless library
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Message Recieved - %u: %s\n", from, msg.c_str());
  checkMessage(msg, from);
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Node on Mesh - nodeID: %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Mesh Connections Modified\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Node Time Adjusted to %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
  pinMode(FLAME, INPUT);
  
//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
