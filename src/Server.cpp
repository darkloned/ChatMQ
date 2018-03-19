/*
 * Created by darkloned.
 * Coursework:
 *  Create secure ZeroMQ IM chat with
 *	authorization and dialog history.
 *
 * See log files for usage example.
 */

#include "Server.h"

int main(int argc, char* argv[])
{
	void* context = zmq_ctx_new();
	void* socket = zmq_socket(context, ZMQ_REP);

	zmq_bind(socket, "tcp://*:4040");

	std::cout << "ChatMQ Server v0.1 by @darkloned started.\n" << std::endl;

	mkdir("data", RWXUSR); // accounts data

	for (;;) 
	{
		DataPacket requestData = receiveDataPacket(socket);

		std::string reply;

		// Well, of course I don't need switch-case for strings, right, C++?
		if (requestData.action == ":create")
		{
			reply = createAccount(requestData);
		}
		else if (requestData.action == ":login")
		{
			reply = loginUser(requestData);
		}
		else if (requestData.action == ":status")
		{
			reply = setUserStatus(requestData);
		}
		else if (requestData.action == ":connect")
		{
			reply = connectRecipient(requestData);
		}
		else if (requestData.action == ":disconnect")
		{
			reply = disconnectRecipient(requestData);
		}
		else if (requestData.action == ":logout" ||
				 requestData.action == ":quit"    )
		{
			reply = logoutUser(requestData);
		}
		else if (requestData.action == ":delete")
		{
			reply = deleteAccount(requestData);
		}

		sendString(socket, reply, false);
	}

	zmq_close(socket);
	zmq_ctx_destroy(context);

	return 0;
}
