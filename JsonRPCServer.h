/*
  JsonRPCServer.h - Simple JSON-RPC Server for Arduino
  Created by Meir Tseitlin, March 5, 2014. This code is based on https://code.google.com/p/ajson-rpc/
  Released under GPLv2 license.
*/

#ifndef JsonRPC_h
#define JsonRPC_h

#include "Arduino.h"
#include <Stream.h>
#include "aJSON.h"

enum JSON_RPC_RET_TYPE {
	JSON_RPC_RET_TYPE_NONE,
	JSON_RPC_RET_TYPE_NUMERIC,
	JSON_RPC_RET_TYPE_STRING	
};

class JsonRPCServer;

typedef void (JsonRPCServer::*JSON_PROC_T)(aJsonObject*);
typedef void (*JSON_PROC_STATIC_T)(JsonRPCServer*, aJsonObject*);

typedef int (JsonRPCServer::*JSON_PROC_NUM_T)(aJsonObject*);
typedef int (*JSON_PROC_NUM_STATIC_T)(JsonRPCServer*, aJsonObject*);

typedef String (JsonRPCServer::*JSON_PROC_STRING_T)(aJsonObject*);
typedef String (*JSON_PROC_STRING_STATIC_T)(JsonRPCServer*, aJsonObject*);


struct Mapping
{
    String name;
	JSON_PROC_STATIC_T callback;
	JSON_RPC_RET_TYPE retType;
};

struct FuncMap
{
    Mapping* mappings;
    unsigned int capacity;
    unsigned int used;
};


#define DECLARE_JSON_PROC(CONTAINER_NAME, METHOD_NAME, RET_TYPE) \
	RET_TYPE METHOD_NAME(aJsonObject* params); \
	static RET_TYPE stat_##METHOD_NAME(CONTAINER_NAME* container, aJsonObject* params) { \
		return container->METHOD_NAME(params); \
	}

#define BEGIN_JSON_REGISTRATION \
	protected: \
		void registerProcs() {

#define REGISTER_JSON_PROC(METHOD_NAME, RET_TYPE) \
	registerMethod(#METHOD_NAME, (JSON_PROC_STATIC_T) &stat_##METHOD_NAME, RET_TYPE)


#define END_JSON_REGISTRATION \
		}

class JsonRPCServer
{
public:
	
	JsonRPCServer(Stream* stream);
	
	void begin(int capacity);
	void process();
protected:
	void registerMethod(String methodName, JSON_PROC_STATIC_T callback, JSON_RPC_RET_TYPE type = JSON_RPC_RET_TYPE_NONE);
	void processMessage(aJsonObject *msg);
	virtual void registerProcs() = 0;
private:
	FuncMap* mymap;
	aJsonStream _jsonStream;
};

#endif
