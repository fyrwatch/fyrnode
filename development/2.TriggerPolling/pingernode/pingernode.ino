// ===========================================================================
// Copyright (C) 2020 Manish Meganathan, Mariyam A.Ghani. All Rights Reserved.
// 
// This file is part of the FyrNode library.
// No part of the FyrNode library can not be copied and/or distributed 
// without the express permission of Manish Meganathan and Mariyam A.Ghani
// ===========================================================================
// Code for node with a ping button, which when pressed
// broadcasts a ping request to the mesh.
//
// Button is attached to D0 (PIN16)
// Baud Rate - 115200
//
// Disregard warning for JCButton library's compatibility.
// Written for NodeMCU 1.0 ESP-12E board
// ===========================================================================

#include "painlessMesh.h"
#include "ArduinoJson.h"
#include "JC_Button.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555
#define   BUTTON_PIN      16

painlessMesh  mesh;
Scheduler userScheduler;
Button button(BUTTON_PIN);

void sendMessage() {
  int pingID = random(0,999);
  
  DynamicJsonDocument doc(1024);
  doc["nodeID"] = mesh.getNodeId();
  doc["pingID"] = pingID;
  doc["data"] = "give response now";
  
  String msg; serializeJson(doc, msg);
  mesh.sendBroadcast(msg);
  Serial.printf("Mesh Pinged with ID %d\n", pingID);
}

// Needed for painless library
void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Message Recieved - %u: %s\n", from, msg.c_str());
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
  button.begin();
  
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
  button.read();
  if (button.wasReleased()){
    sendMessage();
  }
}
