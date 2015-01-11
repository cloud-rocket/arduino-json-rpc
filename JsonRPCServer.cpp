/*
  JsonRPCServer.cpp - Simple JSON-RPC Server for Arduino
  Created by Meir Tseitlin, March 5, 2014. This code is based on https://code.google.com/p/ajson-rpc/
  Released under GPLv2 license.
*/
#include "Arduino.h"
#include "aJSON.h"
#include "JsonRPCServer.h"

JsonRPCServer::JsonRPCServer(Stream* stream): _jsonStream(stream) {
	
}


void JsonRPCServer::begin(int capacity)
{
    mymap = (FuncMap *)malloc(sizeof(FuncMap));
    mymap->capacity = capacity;
    mymap->used = 0;
    mymap->mappings = (Mapping*)malloc(capacity * sizeof(Mapping));
    memset(mymap->mappings, 0, capacity * sizeof(Mapping));
	
	// Register all json procedures (pure virtual - implemented in derivatives with macros)
	registerProcs();
}

void JsonRPCServer::registerMethod(String methodName, JSON_PROC_STATIC_T callback, JSON_RPC_RET_TYPE type)
{
    // only write keyvalue pair if we allocated enough memory for it
    if (mymap->used < mymap->capacity)
    {
	    Mapping* mapping = &(mymap->mappings[mymap->used++]);
	    mapping->name = methodName;
	    mapping->callback = callback;
		mapping->retType = type;
    }
}


void JsonRPCServer::processMessage(aJsonObject *msg)
{
    aJsonObject* method = aJson.getObjectItem(msg, "method");
    if (!method)
    {
	// not a valid Json-RPC message
        Serial.flush();
        return;
    }
    
    aJsonObject* params = aJson.getObjectItem(msg, "params");
    if (!params)
    {
	Serial.flush();
	return;
    }
    
    String methodName = method->valuestring;
    for (int i=0; i<mymap->used; i++)
    {
        Mapping* mapping = &(mymap->mappings[i]);
        if (methodName.equals(mapping->name))
        {
			switch (mapping->retType) {
			case	JSON_RPC_RET_TYPE_NONE:
				mapping->callback(this, params);
				break;
			case	JSON_RPC_RET_TYPE_NUMERIC:
				{
					JSON_PROC_NUM_STATIC_T numCallback = (JSON_PROC_NUM_STATIC_T) mapping->callback;
					
					int ret = numCallback(this, params);
					
					// Create answer
					aJsonObject *answer = aJson.createObject();
					aJson.addItemToObject(answer, "result", aJson.createItem((int)ret));
					aJson.addItemToObject(answer, "error", aJson.createNull());
					
					// Send answer
					aJson.print(answer, &_jsonStream);
					
					// Delete answer
					aJson.deleteItem(answer);
				}
				break;
			case	JSON_RPC_RET_TYPE_STRING:
				{
					//String ret = mapping->callback(this, params);
					JSON_PROC_STRING_STATIC_T stringCallback = (JSON_PROC_STRING_STATIC_T) mapping->callback;
					
					String ret = stringCallback(this, params);
					
					// Create answer
					aJsonObject *answer = aJson.createObject();
					aJson.addItemToObject(answer, "result", aJson.createItem(ret.c_str()));
					aJson.addItemToObject(answer, "error", aJson.createNull());
					
					// Send answer
					aJson.print(answer, &_jsonStream);
					
					// Delete answer
					aJson.deleteItem(answer);
					
				}
				break;
			}
			
			return;
		}
    }
}

void JsonRPCServer::process() {
	
	/* add main program code here */
	if (_jsonStream.available()) {

		// skip any accidental whitespace like newlines
		_jsonStream.skip();
	}

	if (_jsonStream.available()) {
		
		aJsonObject *msg = aJson.parse(&_jsonStream);
		if (msg) {
			processMessage(msg);
			aJson.deleteItem(msg);
			} else {
			_jsonStream.flush();
		}
	}
	
}
