# fyrnode
## A platform to design, configure and deploy nodes on the fyrmesh

### Contributors
- Manish Meganathan
- Mariyam A. Ghani

This repository will contain the following:    
- **A Cloud-Native Microservice Application**  
  The cloud app will be a python flask/bottle app that exposes an endpoint that accepts a JSON configuration of a node.
  The app generates a sketch code for the provided configuration and responds to client with it. The app is expected to be hosted on the GCP Cloud Run platform as a serverless container based microservice.
- **An Arduino Library for the ESP8266**  
  The Arduino library contains essential code for components such as the mesh messaging protocol, sensor configuration abstraction and modular callbacks for the painless mesh library. It is intended to compatible with the ESP8266 NodeMCU platform.
- **A Pre-Configured Sample Collection**  
  The collection contains the code firmware, hardware schematics and 3D models enclosures for a set of pre-configured sensor nodes. 
- **A GUI Application**  
  A GUI application that exposes the underlying platform through a form-like interface that can accept the configuration of the sensor node and generate relevant details and firmware for it.
- **A Development Collection**  
  A collection of experimental code and development notes that track the major development milestones of this platform.