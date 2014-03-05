/*
  JsonRPC Library - ToggleLED
 
 Demonstrates how to toggle the status of a LED by sending a
 json message to the arduino over the serial connection.
 
 The following python script can be used to toggle the LED
 after the sketch has been uploaded to the arduino:
 
 import sys
 import serial
 import time
 
 port = '/dev/ttyACM0'
 ser = serial.Serial(port, 115200)
 
 # give the serial connection 2 seconds to settle
 time.sleep(2)
 
 ser.write('{"method": "init", "params": {} }')
 
 time.sleep(1)
 
 ser.write('{"method": "toggleLED", "params": {"status": true} }')
 time.sleep(5)
 ser.write('{"method": "toggleLED", "params": {"status": false} }')
 
 # wait 2 seconds before closing the serial connection
 time.sleep(2)
 ser.close()

 */

// include the aJSON library
#include <aJSON.h>
// include the JsonRPC library
#include <JsonRPCServer.h>



class TargetController: public JsonRPCServer {
public:
	TargetController(Stream* stream);

	DECLARE_JSON_PROC(TargetController, init, int);
	DECLARE_JSON_PROC(TargetController, toggleLED, String);
	

	BEGIN_JSON_REGISTRATION
		REGISTER_JSON_PROC(init, JSON_RPC_RET_TYPE_NUMERIC);
		REGISTER_JSON_PROC(toggleLED, JSON_RPC_RET_TYPE_STRING);
	END_JSON_REGISTRATION
	

private:
	
	// on most arduino boards, pin 13 is connected to a LED
	int _led;
	
};





TargetController::TargetController(Stream* stream): JsonRPCServer(stream), _led(13) {
	// TODO Auto-generated constructor stub

}

int TargetController::init(aJsonObject* params) {

  // initialize the digital pin as an output
  pinMode(_led, OUTPUT);

	return true;
}

String TargetController::toggleLED(aJsonObject* params)
{
	aJsonObject* statusParam = aJson.getObjectItem(params, "status");
	boolean requestedStatus = statusParam -> valuebool;
	
	if (requestedStatus)
	{
		digitalWrite(_led, HIGH);
		return "led HIGH";
	}
	else
	{
		digitalWrite(_led, LOW);
		return "led LOW";
	}
}




TargetController jsonController(&Serial);


void setup()
{
  // start up the serial interface
  Serial.begin(115200);
  Serial.println("Initializing JSON RPC server");

  /* add setup code here */

	jsonController.begin(2);
}

void loop()
{

	jsonController.process();
}
