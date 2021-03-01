// ===========================================================================
// Copyright (C) 2020 Manish Meganathan, Mariyam A.Ghani. All Rights Reserved.
// 
// This file is part of the FyrNode library.
// No part of the FyrNode library can not be copied and/or distributed 
// without the express permission of Manish Meganathan and Mariyam A.Ghani
// ===========================================================================
// Code for sensor node that simply transmits a greeting.
//
// Baud Rate - 115200
// Message Broadcast every 5 seconds
//
// Written for NodeMCU 1.0 ESP-12E board
// ===========================================================================

#include "painlessMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval(TASK_SECOND * 5);
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
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

//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
