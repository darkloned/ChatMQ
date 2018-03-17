/*
 * Created by darkloned.
 * Coursework:
 *  Create secure ZeroMQ IM chat with
 	authorization and dialog history.
 *
 * See log files for usage example.
 */

// TODO: color lines

#include <iostream>
#include <cctype>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <unistd.h>
#include <termios.h>

#include "sha256.h"

#include "DataPacket.h"

void printHelp()
{
	std::cout << std::endl;
	std::cout << ":create ~ :delete - create or delete account" << std::endl;
	std::cout << ":login  ~ :logout - authorization management" << std::endl;
	std::cout << ":status\t\t  - set user status (i.e. \"Hello, World!\")" << std::endl;
	std::cout << ":connect\t  - open chat with selected user" << std::endl;
	std::cout << ":quit\t\t  - exit application" << std::endl;
	std::cout << ":help\t\t  - print this manual\n" << std::endl;
}

// check if username contains NOT only [a-z, A-Z, 0-9]
bool incorrectUsername(std::string username)
{
	for (uint i = 0; i < username.length(); ++i)
	{
		if (!std::isdigit(username[i]) && !std::isalpha(username[i]))
		{
			return true;
		}
	}

	return false;
}

// for Unix-like OS
int getch()
{
    int c;
    struct termios t_old, t_new;

    tcgetattr(STDIN_FILENO, &t_old);

    t_new = t_old;
    t_new.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

    c = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &t_old);

    return c;
}

void getpass(std::string& password)
{
	const char BACKSPACE = 127;
	const char ENTER = 10;

	unsigned char c = 0;

	while ((c = getch()) != ENTER)
	{
		if (c == BACKSPACE)
		{
			if (password.length() != 0)
			{
				 std::cout << "\b \b";

				 password.resize(password.length() - 1);
			}
		}
		else
		{
			password += c;
			
			std::cout << '*';
		}
	}

	std::cout << std::endl;
}

std::string getCurrentTime()
{
	auto chronoTime = std::chrono::system_clock::now();
	std::time_t currentTime = std::chrono::system_clock::to_time_t(chronoTime);
	std::stringstream ss;

	ss << std::put_time(std::localtime(&currentTime), "%d-%m-%Y at %H:%M");

	return ss.str();
}

int main(int argc, char* argv[])
{
	void* context = zmq_ctx_new();
	void* socket = zmq_socket(context, ZMQ_REQ);

	zmq_connect(socket, "tcp://localhost:4040");

	std::cout << "Welcome to ChatMQ v0.1 by @darkloned." << std::endl;
	printHelp();

	std::string username = "", passwordHash; // current logged in user

	for (;;) 
	{
		DataPacket requestData;

		// temporary strings to store and process user input
		std::string login, password; 

		std::cout << "> ";
		std::getline(std::cin, requestData.action);

		// Well, of course I don't need switch-case for strings, right, C++?
		if (requestData.action == ":create")
		{
			if (!isEmpty(username))
			{
				std::cout << "\nLog out first to create new account.\n" << std::endl;

				continue;
			}
			else
			{
				for (;;)
				{
					std::cout << "\nLogin: ";
					std::getline(std::cin, login);
					trim(login);

					if (isEmpty(login))
					{
						std::cout << "Empty username entered." << std::endl;
					}
					else if (incorrectUsername(login))
					{
						std::cout << "Username should contain only [a-z, A-Z, 0-9]." << std::endl;
					}
					else
					{
						requestData.loginHash = sha256(login);

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
			}
		}
		else if (requestData.action == ":login")
		{
			if (!isEmpty(username))
			{
				std::cout << "\nAlready logged in as @" << username << ".\n" << std::endl;

				continue;
			}
			else
			{
				std::cout << "\nLogin: ";
				std::getline(std::cin, login);
				trim(login);

				std::cout << "Password: ";
				getpass(password);

				requestData.loginHash = sha256(login);
				requestData.passwordHash = sha256(password);
				requestData.time = getCurrentTime();
			}
		}
		else if (requestData.action == ":status")
		{
			if (isEmpty(username))
			{
				std::cout << "\nLog in first to set new status.\n" << std::endl;

				continue;
			}
			else
			{
				requestData.loginHash = sha256(username);
				requestData.passwordHash = passwordHash;

				std::cout << "\nEnter new status: ";
				std::getline(std::cin, requestData.userStatus);
			}
		}
		else if (requestData.action == ":connect")
		{
			if (isEmpty(username))
			{
				std::cout << "\nLog in first to open chat.\n" << std::endl;

				continue; 
			}
			else
			{
				std::string recipient;

				std::cout << "\nEnter recipient username: @";
				std::getline(std::cin, recipient);

				requestData.loginHash = sha256(username);
				requestData.passwordHash = passwordHash;
				requestData.recipientHash = sha256(recipient);
			}
		}
		else if (requestData.action == ":logout")
		{
			if (isEmpty(username))
			{
				std::cout << "\nYou are not logged in.\n" << std::endl;

				continue;
			}
			else
			{
				requestData.loginHash = sha256(username);
				requestData.passwordHash = passwordHash;
			}
		}
		else if (requestData.action == ":delete")
		{
			if (isEmpty(username))
			{
				std::cout << "\nLog in first to delete your account.\n" << std::endl;

				continue;
			}
			else
			{
				std::string response;

				std::cout << "\nThis will ERASE your account and PROHIBIT any further access to current chats." << std::endl;
				std::cout << "Do you really wish to proceed? [yes/No]: ";
				std::getline(std::cin, response);

				if (response != "yes")
				{
					std::cout << "Account deletion aborted.\n" << std::endl;

					continue;
				}

				std::cout << "Password: ";
				getpass(password);

				requestData.loginHash = sha256(username);
				requestData.passwordHash = sha256(password);
			}
		}
		else if (requestData.action == ":quit")
		{
			if (isEmpty(username))
			{
				// no need to log out, exit here
				std::cout << "\nBye!\n" << std::endl;

				return 0;
			}

			requestData.loginHash = sha256(username);
			requestData.passwordHash = sha256(passwordHash);
		}
		else if (requestData.action == ":help")
		{
			printHelp();

			continue;
		}
		else
		{
			std::cout << "Unknown request." << std::endl<< "Use :help in order to print the list of options.\n" << std::endl;

			continue;
		}

		std::cout << std::endl;

		sendDataPacket(socket, requestData);

		std::string reply = receiveServerReply(socket);

		if (requestData.action == ":create")
		{
			if (reply == "OK")
			{
				username = login;
				passwordHash = requestData.passwordHash;

				std::cout << "Hello, @" << username << "! Your account has been created." << std::endl;
			}
			else
			{
				std::cout << reply << std::endl;
			}
		}
		else if (requestData.action == ":login")
		{
			if (contains(reply, "-")) // reply is last login time
			{
				username = login;
				passwordHash = requestData.passwordHash;

				std::cout << "Hello, @" << username << "!" << std::endl;
				std::cout << "Last login: " << reply << std::endl;
			}
			else
			{
				std::cout << reply << std::endl;
			}
		}
		else if (requestData.action == ":logout")
		{
			std::cout << "Bye, @" << username << "!" << std::endl;

			username = "";
			passwordHash = "";
		}
		else if (requestData.action == ":quit") // user was logged out by server
		{
			std::cout << "Bye, @" << username << "!\n" << std::endl;

			return 0;
		}

		std::cout << std::endl;
	}

	zmq_close(socket);
	zmq_ctx_destroy(context);

	return 0;
}
