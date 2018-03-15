#ifndef DATAPACKET_H
#define DATAPACKET_H

#include "zmq.h"

// enum Action { CREATE, LOGIN, STATUS, CONNECT, LOGOUT, DELETE, QUIT };

// TODO: make class DataPacket with method 'send()'
// TODO: http://zeromq.org/build:encryption

typedef struct DataPacket
{
	std::string action, loginHash, passwordHash, recipientHash, userStatus;

} DataPacket;

bool isEmpty(std::string s)
{
	return s.size() == 0;
}

void sendString(void* socket, std::string s, bool sendMore)
{
	zmq_msg_t zmq_s;

	zmq_msg_init_size(&zmq_s, s.length() + 1);

	memcpy(zmq_msg_data(&zmq_s), s.c_str(), s.length() + 1);

	zmq_msg_send(&zmq_s, socket, sendMore ? ZMQ_SNDMORE : 0);
	zmq_msg_close(&zmq_s);
}

std::string receiveString(void* socket)
{
	zmq_msg_t zmq_s;

	zmq_msg_init(&zmq_s);
	zmq_msg_recv(&zmq_s, socket, 0);

	char* s = (char*) zmq_msg_data(&zmq_s);

	zmq_msg_close(&zmq_s);

	return s == NULL ? "" : std::string(s);
}

void sendDataPacket(void* socket, DataPacket requestData)
{
	sendString(socket, requestData.action, true);
	sendString(socket, requestData.loginHash, true);
	sendString(socket, requestData.passwordHash, true);
	sendString(socket, requestData.recipientHash, true);
	sendString(socket, requestData.userStatus, false);
}

DataPacket receiveDataPacket(void* socket)
{
	DataPacket dp;

	dp.action 		 = receiveString(socket);
	dp.loginHash 	 = receiveString(socket);
	dp.passwordHash  = receiveString(socket);
	dp.recipientHash = receiveString(socket);
	dp.userStatus 	 = receiveString(socket);

	return dp;
}

std::string receiveServerReply(void* socket)
{
	zmq_msg_t zmqReply;

	zmq_msg_init(&zmqReply);
	zmq_msg_recv(&zmqReply, socket, 0);

	char* reply = (char*) zmq_msg_data(&zmqReply);

	zmq_msg_close(&zmqReply);

	return std::string(reply);
}

#endif
