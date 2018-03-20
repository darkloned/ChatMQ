#include "PrintServerReply.h"

void replyCreatingAccount(std::string reply, DataPacket requestData, std::string& username, std::string& passwordHash)
{
	if (reply == "OK")
	{
		passwordHash = requestData.passwordHash;

		std::cout << "Hello, @" << username << "! Your account has been created." << std::endl;
	}
	else
	{
		username = "";

		std::cout << reply << std::endl;
	}
}

void replyLoggingUserIn(std::string reply, DataPacket requestData, std::string& username, std::string& passwordHash)
{
	if (contains(reply, "-")) // reply is last login time
	{
		passwordHash = requestData.passwordHash;

		std::cout << "Hello, @" << username << "!" << std::endl;
		std::cout << "Last login: " << reply << std::endl;
	}
	else
	{
		username = "";

		std::cout << reply << std::endl;
	}
}

void replySettingUserStatus(std::string reply)
{
	if (reply == "OK")
	{
		std::cout << "New status has been set." << std::endl;
	}
	else
	{
		std::cout << reply << std::endl;
	}
}

void replyConnectingRecipient(std::string reply, std::string& recipient)
{
	if (contains(reply, "status")) // reply is recipient info
	{
		std::cout << "@" << recipient << reply << std::endl;
	}
	else
	{
		recipient = "";

		std::cout << reply << std::endl;
	}
}

void replyDisconnectingRecipient(std::string reply, std::string& recipient)
{
	if (reply == "OK")
	{
		std::cout << "@" << recipient << " has been disconnected." << std::endl;

		recipient = "";
	}
	else
	{
		std::cout << reply << std::endl;
	}
}

void replyLoggingUserOut(std::string reply, std::string& username, std::string& passwordHash)
{
	if (reply == "OK")
	{
		std::cout << "Bye, @" << username << "!" << std::endl;

		username = "";
		passwordHash = "";
	}
	else
	{
		std::cout << reply << std::endl;
	}
}

void replyDeletingAccount(std::string reply, std::string& username, std::string& passwordHash)
{
	if (reply == "OK")
	{
		username = "";
		passwordHash = "";

		std::cout << "Your account has been deleted." << std::endl;
	}
	else
	{
		std::cout << reply << std::endl;
	}
}

void replyQuittingClientApp(std::string reply, std::string username)
{
	if (reply == "OK")
	{
		std::cout << "Bye, @" << username << "!\n" << std::endl;

		exit(0);
	}
	else
	{
		std::cout << reply << "\n" << std::endl;
	}
} 
