#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <memory.h>

#include "zmq.h"

#include "StringUtils.h"

// TODO: make class DataPacket with method 'send()'
// TODO: http://zeromq.org/build:encryption

typedef struct DataPacket
{
	std::string action, usernameHash, passwordHash, recipientHash, userStatus, time;

} DataPacket;

void sendString(void* socket, std::string s, bool sendMore);
std::string receiveString(void* socket);

void sendDataPacket(void* socket, DataPacket requestData);
DataPacket receiveDataPacket(void* socket);

std::string receiveServerReply(void* socket);


#endif
