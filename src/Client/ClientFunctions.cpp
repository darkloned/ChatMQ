#include "ClientFunctions.h"

bool createAccount(DataPacket& requestData, std::string& username)
{
	if (!isEmpty(username))
	{
		std::cout << "\nLog out first to create new account.\n" << std::endl;

		return false;
	}
	else
	{
		for (;;)
		{
			std::cout << "\nLogin: ";
			std::getline(std::cin, username);
			trim(username);

			if (isEmpty(username))
			{
				std::cout << "Empty username entered." << std::endl;
			}
			else if (incorrectUsername(username))
			{
				std::cout << "Username should contain only [a-z, A-Z, 0-9]." << std::endl;
			}
			else
			{
				requestData.usernameHash = sha256(username);

				break;
			}
		}

		for (;;)
		{
			std::string pwd1, pwd2;

			std::cout << "Password: ";
			getpass(pwd1);
			std::cout << "Repeat password: ";
			getpass(pwd2);

			if (pwd1 != pwd2)
			{
				std::cout << "Passwords do not match, please try again" << std::endl;
			}
			else
			{
				requestData.passwordHash = sha256(pwd1);

				break;
			}
		}

		requestData.time = getCurrentTime();

		return true;
	}
}

bool loginUser(DataPacket& requestData, std::string& username)
{
	if (!isEmpty(username))
	{
		std::cout << "\nAlready logged in as @" << username << ".\n" << std::endl;

		return false;
	}
	else
	{
		std::cout << "\nLogin: ";
		std::getline(std::cin, username);
		trim(username);

		std::string password;
		std::cout << "Password: ";
		getpass(password);

		requestData.usernameHash = sha256(username);
		requestData.passwordHash = sha256(password);
		requestData.time = getCurrentTime();

		return true;
	}
}

bool setUserStatus(DataPacket& requestData, std::string username, std::string passwordHash)
{
	if (isEmpty(username))
	{
		std::cout << "\nLog in first to set new status.\n" << std::endl;

		return false;
	}
	else
	{
		requestData.usernameHash = sha256(username);
		requestData.passwordHash = passwordHash;

		std::cout << "\nEnter new status: ";
		std::getline(std::cin, requestData.userStatus);

		return true;
	}
}

bool connectRecipient(DataPacket& requestData, std::string username, std::string passwordHash, std::string& recipient)
{
	if (isEmpty(username))
	{
		std::cout << "\nLog in first to open chat.\n" << std::endl;

		return false; 
	}
	else if (!isEmpty(recipient))
	{
		std::cout << "\nYou're connected already.\nDisconnect first to choose another user." << std::endl;

		return false;
	}
	else
	{
		std::cout << "\nEnter recipient username: @";
		std::getline(std::cin, recipient);

		requestData.usernameHash = sha256(username);
		requestData.passwordHash = passwordHash;
		requestData.recipientHash = sha256(recipient);

		return true;
	}
}

bool disconnectRecipient(DataPacket& requestData, std::string username, std::string passwordHash, std::string recipient)
{
	if (isEmpty(recipient))
	{
		std::cout << "\nYou are not connected to anyone.\n" << std::endl;

		return false;
	}
	else
	{
		requestData.usernameHash = sha256(username);
		requestData.passwordHash = passwordHash;

		return true;
	}
}

bool logoutUser(DataPacket& requestData, std::string username, std::string passwordHash)
{
	if (isEmpty(username))
	{
		std::cout << "\nYou are not logged in.\n" << std::endl;

		return false;
	}
	else
	{
		requestData.usernameHash = sha256(username);
		requestData.passwordHash = passwordHash;

		return true;
	}
}

bool deleteAccount(DataPacket& requestData, std::string username)
{
	if (isEmpty(username))
	{
		std::cout << "\nLog in first to delete your account.\n" << std::endl;

		return false;
	}
	else
	{
		std::string response;
		std::cout << "\nThis will erase all your chat data PERMANENTLY." << std::endl;
		std::cout << "Do you really wish to proceed? [yes/No]: ";
		std::getline(std::cin, response);

		if (response != "yes")
		{
			std::cout << "Account deletion aborted.\n" << std::endl;

			return false;
		}

		std::string password;
		std::cout << "Password: ";
		getpass(password);

		requestData.usernameHash = sha256(username);
		requestData.passwordHash = sha256(password);

		return true;
	}
}

void quitClientApp(DataPacket& requestData, std::string username, std::string passwordHash)
{
	if (isEmpty(username))
	{
		// no need to log out, exit here
		std::cout << "\nBye!\n" << std::endl;

		exit(0);
	}

	requestData.usernameHash = sha256(username);
	requestData.passwordHash = passwordHash;
}

void printHelp()
{
	std::cout << std::endl;
	std::cout << ":create  ~ :delete      - create or delete account" << std::endl;
	std::cout << ":login   ~ :logout      - authorization management" << std::endl;
	std::cout << ":connect ~ :disconnect  - open chat with selected user" << std::endl;
	std::cout << ":status\t\t\t- set user status (i.e. \"Hello, World!\")" << std::endl;
	std::cout << ":quit\t\t\t- exit application" << std::endl;
	std::cout << ":help\t\t\t- print this manual\n" << std::endl;
}

void unknownRequest()
{
	std::cout << "Unknown request." << std::endl;
	std::cout << "Use :help in order to print the list of options.\n" << std::endl;
} 
