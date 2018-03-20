/*
 * Created by darkloned.
 * Coursework:
 *  Create secure ZeroMQ IM chat with
 	authorization and dialog history.
 *
 * See log files for usage example.
 */

// TODO: color lines

#include "ClientFunctions.h"
#include "PrintServerReply.h"

int main(int argc, char* argv[])
{
	void* context = zmq_ctx_new();
	void* socket = zmq_socket(context, ZMQ_REQ);

	zmq_connect(socket, "tcp://localhost:4040");

	std::cout << "Welcome to ChatMQ v0.1 by @darkloned." << std::endl;
	printHelp();

	std::string username  = "", passwordHash; // current logged in user
	std::string recipient = "";

	for (;;) 
	{
		DataPacket requestData;

		std::cout << "> ";
		std::getline(std::cin, requestData.action);

		// Well, of course I don't need switch-case for strings, right, C++?
		if (requestData.action == ":create")
		{
			if (!createAccount(requestData, username))
				continue;
		}
		else if (requestData.action == ":login")
		{
			if (!loginUser(requestData, username))
				continue;
		}
		else if (requestData.action == ":status")
		{
			if (!setUserStatus(requestData, username, passwordHash))
				continue;
		}
		else if (requestData.action == ":connect")
		{
			if (!connectRecipient(requestData, username, passwordHash, recipient))
				continue;
		}
		else if (requestData.action == ":disconnect")
		{
			if (!disconnectRecipient(requestData, username, passwordHash, recipient))
				continue;
		}
		else if (requestData.action == ":logout")
		{
			if (!logoutUser(requestData, username, passwordHash))
				continue;
		}
		else if (requestData.action == ":delete")
		{
			if (!deleteAccount(requestData, username))
				continue;
		}
		else if (requestData.action == ":quit")
		{
			quitClientApp(requestData, username, passwordHash);
		}
		else if (requestData.action == ":help")
		{
			printHelp();
			continue;
		}
		else
		{
			unknownRequest();
			continue;
		}

		std::cout << std::endl;

		sendDataPacket(socket, requestData);

		std::string reply = receiveServerReply(socket);

		// Here we go again...
		if (requestData.action == ":create")
		{
			replyCreatingAccount(reply, requestData, username, passwordHash);
		}
		else if (requestData.action == ":login")
		{
			replyLoggingUserIn(reply, requestData, username, passwordHash);
		}
		else if (requestData.action == ":status")
		{
			replySettingUserStatus(reply);
		}
		else if (requestData.action == ":connect")
		{
			replyConnectingRecipient(reply, recipient);
		}
		else if (requestData.action == ":disconnect")
		{
			replyDisconnectingRecipient(reply, recipient);
		}
		else if (requestData.action == ":logout")
		{
			replyLoggingUserOut(reply, username, passwordHash);
		}
		else if (requestData.action == ":delete")
		{
			replyDeletingAccount(reply, username, passwordHash);
		}
		else if (requestData.action == ":quit") // user was logged out by server
		{
			replyQuittingClientApp(reply, username);
		}

		std::cout << std::endl;
	}

	zmq_close(socket);
	zmq_ctx_destroy(context);

	return 0;
}
