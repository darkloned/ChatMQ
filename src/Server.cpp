/*
 * Created by darkloned.
 * Coursework:
 *  Create secure ZeroMQ IM chat with
 	authorization and dialog history.
 *
 * See log files for usage example.
 */

#include "Server.h"

int main(int argc, char* argv[])
{
	void* context = zmq_ctx_new();
	void* socket = zmq_socket(context, ZMQ_REP);

	zmq_bind(socket, "tcp://*:4040");

	std::cout << "ChatMQ Server v0.1 by @darkloned started." << std::endl << std::endl;

	mkdir("data", RWXUSR); // accounts data

	for (;;) 
	{
		DataPacket requestData = receiveDataPacket(socket);

		std::string reply;

		// Well, of course I don't need switch-case for strings, right, C++?
		if (requestData.action == ":create")
		{
			reply = createNewAccount(requestData);
		}
		else if (requestData.action == ":login")
		{
			reply = authorize(requestData);
		}

		sendString(socket, reply, false);
	}

	zmq_close(socket);
	zmq_ctx_destroy(context);

	return 0;
}
