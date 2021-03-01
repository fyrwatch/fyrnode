# FyrNode Library  
![FyrNode Banner](../fyrnode_banner.png)

**Version: 0.1.0**  
**Platform: Arduino ESP8266**  
**Language: Arduino C/C++**

### **Contributors**
- **Manish Meganathan**
- **Mariyam A. Ghani**
  
An Arduino library that contains firmware code for sensor nodes and control nodes that run on the FyrMesh Platfrom. Contains abstraction tools to support a modular sensor hardware configurations, callbacks to support the painlessMesh library, loop runtimes to check button statuses and a custom message delivery protocol.  
It is intended to be compatible with the *ESP8266 NodeMCU* boards.  

The library is based on the [**painlessMesh**](https://github.com/gmag11/painlessMesh) library that implements a mesh network of nodes based on the ESP8266 and ESP32 platforms. This library combines this functionality along with its custom JSON-based command messaging protocol and modular hardware configuration support to abstract the FyrMesh platforms requirements into a simple collection classes that achieve all the required functionality behind the scenes.

## Library Dependencies
- [painlessMesh](https://github.com/gmag11/painlessMesh)
- [ArduinoJSON](https://github.com/bblanchon/ArduinoJson)
- [JC_Button](https://github.com/JChristensen/JC_Button)

## API
The library contains two classes **FyrNode** and **FyrNodeControl**. They behave as the sensor nodes and the control node for the FyrMesh platform respectively. The hardware configuration of the node is specified using a collection of global values made available to the library using the ``extern`` keyword.

This library while designed to be hackable and modular. Is intended to be used as is on the nodes themselves. The only level of customization on the nodes is expected to be with the configuration value set to support different types of hardware configurations. New functionality should be implemented directly into the library and not on the sketch file of the node.

### ``FyrNode``
The ``FyrNode`` object is initialised as mentioned below. The hardware configuration is not specfied at the time of intialisation of this object, but is rather accessed from the collection of global variables. See more in the Hardware Configuration Section.
```
FyrNode sensornode;
```

The ``FyrNode`` object has the following member methods.
- ``begin()``  
  This method is used in the ``void setup()`` function of the Arduino Sketch. It initialises all the sensors, buttons, interfaces and initiates the mesh.
- ``update()``
  This method is used in the ``void loop()`` function of the Arduino Sketch. It runs the ``mesh.update()`` runtime in the backend along with checking for connection states, new messages and button states. 

### ``FyrNodeControl``
The ``FyrNodeControl`` object is initialised as mentioned below. The hardware configuration is not specfied at the time of intialisation of this object, but is rather accessed from the collection of global variables. See more in the Hardware Configuration Section.
```
FyrNodeControl controlnode;
```

The ``FyrNode`` object has the following member methods.
- ``begin()``  
  This method is used in the ``void setup()`` function of the Arduino Sketch. It initialises the SoftwareSerial interface with the Raspberry Pi Controller along with the buttons and initiates the mesh.
- ``update()``
  This method is used in the ``void loop()`` function of the Arduino Sketch. It runs the ``mesh.update()`` runtime in the backend along with checking for connection states, new messages and button states. 

## Configuration Values  
The FyrNode library supports a modular approach to node design i.e. every node can be built and configured differently and the mesh can still function as a whole. This is achieved by embedding some values as global constants in the Arduino Sketch, which is then accessed by the library using the ``extern`` keyword that tells the linker to fetch the symbol from another part of the program.  

The configuration symbols are as follows:
- ``MESH_SSID``  
  This ``String`` value determines the SSID name of the mesh AP configuration.
- ``MESH_PSWD``  
  This ``String`` value determines the password of the mesh AP configuration.
- ``MESH_PORT``  
  This ``int`` value determines the port on which the node has to listen to for mesh messages.
- ``DHTTYP``  
  This ``int`` value determines the sensor ID attached to the **DHT** interface. Refer to the *Sensor Value Codes* section for sensor values.
- ``DHTPIN``  
  This ``int`` value determines the pin on which the **DHT** sensor is attached. Refer to *NodeMCU Pin Numbering* section for pin values.
- ``GASTYP``  
  This ``int`` value determines the sensor ID attached to the **GAS** interface. Refer to the *Sensor Value Codes* section for sensor values.
- ``GASPIN``  
- This ``int`` value determines the pin on which the **GAS** sensor is attached. Refer to *NodeMCU Pin Numbering* section for pin values.
- ``FLMTYP``  
  This ``int`` value determines the sensor ID attached to the **FLM** interface. Refer to the *Sensor Value Codes* section for sensor values.
- ``FLMPIN``  
  This ``int`` value determines the pin on which the **FLM** sensor is attached. Refer to *NodeMCU Pin Numbering* section for pin values.
- ``PINGER``  
  This ``bool`` value determines whether a button is attached to the **PINGER** interface. Pressing this button broadcasts a pingresponse command to the mesh.
- ``PINGERPIN``  
  This ``int`` value determines the pin on which the **PINGER** button is attached. Refer to *NodeMCU Pin Numbering* section for pin values.
- ``CONNECT``  
  This ``bool`` value determines whether a button is attached to the **CONNECT** interface. Pressing this button toggles the connectivity of the nodeo to mesh.
- ``CONNECTPIN``  
  This ``int`` value determines the pin on which the **CONNECT** button is attached. Refer to *NodeMCU Pin Numbering* section for pin values.
- ``SRLRX``  
  This ``int`` value determines the pin on which the RX is attached for the Software Serial interface. Only used for ``FyrNodeControl``. Refer to *NodeMCU Pin Numbering* section for pin values.
- ``SRLTX``  
  This ``int`` value determines the pin on which the TX is attached for the Software Serial interface. Only used for ``FyrNodeControl``. Refer to *NodeMCU Pin Numbering* section for pin values.

### **Sensor Value Codes**  
**0 = No Attachment**  
**2 = MQ2 (GAS)**    
**11 = DHT11 (DHT)**   
**16 = SEN16 (FLM)**
**22 = DHT22 (DHT)**  
*all other values are currently unassigned.*

### **NodeMCU Pin Numbering**  
**D0  = 16**   
**D1  = 5**  
**D2  = 4**  
**D3  = 0**  
**D4  = 2**  
**D5  = 14**  
**D6  = 12**  
**D7  = 13**  
**D8  = 15**  
**D9  = 3**  
**D10 = 1**  
**A0  = 17**    
*refer to [this](https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h) for more information about NodeMCU pins*  

## Example Sketch 
An example program that uses the ``FyrNode`` class to define a sensor mesh that has DHT11 sensor attached at pin D1.
```
#include "fyrnode.h"

String MESH_SSID = "whateverYouLike";
String MESH_PSWD = "somethingSneaky";
int MESH_PORT = 5555;

int DHTTYP = 11;		//DHT Attached to DHT11 Sensor
int DHTPIN = 5;			//DHT Attached at Pin D1 (GPIO5)

int GASTYP = 0;			//GAS Attached to None
int GASPIN = 99;		//GAS Attached at None

int FLMTYP = 0;			//FLM Attached to None
int FLMPIN = 99;		//FLM Attached at None

bool PINGER = false;	//PINGER Attached = false
int PINGERPIN = 99;		//PINGER Attached at None

bool CONNECT = false;	//CONNECT Attached = false
int CONNECTPIN = 99;	//CONNECT Attached at None

int SRLRX = 99;			//SRLRX Attached to None
int SRLTX = 99;			//SRLTX Attached to None

FyrNode meshnode;

void setup() {
    meshnode.begin();
}
void loop() {
    meshnode.update();
}
```