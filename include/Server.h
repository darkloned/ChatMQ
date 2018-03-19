#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <fstream>
#include <limits>
#include <cstring>
#include "dirent.h"
#include <sys/stat.h>

#include "DataPacket.h"

#define RWXUSR S_IRUSR | S_IWUSR | S_IXUSR

void sysbash(std::string cmd, std::string filepath)
{
	std::system((cmd + " " + filepath).c_str());
}

void setFileLine(std::string filepath, int number, std::string line)
{
	sysbash("sed -i \'" + std::to_string(number) + "s/.*/\'\"" + line + "\"\'/\'", filepath);
}

std::string getFileLine(std::fstream& file, int number)
{
    file.seekg(std::ios::beg);

    for(int i = 0; i < number - 1; ++i)
    {
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::string line;
    std::getline(file, line);

    return line;
}

enum DataLine { PASSWORD_HASH = 1,
				LAST_LOGIN_TIME, 
				LOGIN_ATTEMPTS_LEFT,
				ONLINE_STATUS,
				USER_STATUS,
				CURRENT_RECIPIENT };

std::string createAccount(DataPacket requestData)
{
	if (mkdir(("data/" + requestData.loginHash).c_str(), RWXUSR) == -1)
	{
		if (errno == EEXIST)
		{
			return "User with such login already exists.\nPlease, choose another one.";
		}

		return "Error has occured while creating account.";
	}
	else
	{
		mkdir(("data/" + requestData.loginHash + "/dialogs").c_str(), RWXUSR);

		std::ofstream userDataOut;
		std::string userDataFilePath = "data/" + requestData.loginHash + "/user.dat";

		userDataOut.open(userDataFilePath, std::ios::out | std::ios::app);

		userDataOut << requestData.passwordHash << std::endl; // setFileLine(userDataFilePath, PASSWORD_HASH, requestData.passwordHash);
		userDataOut << requestData.time 		<< std::endl; // setFileLine(userDataFilePath, LAST_LOGIN_TIME, requestData.time);
		userDataOut << "3" 						<< std::endl; // setFileLine(userDataFilePath, LOGIN_ATTEMPTS_LEFT, "3");
		userDataOut << "online"					<< std::endl; // setFileLine(userDataFilePath, LOGIN_STATUS, "online");
		userDataOut << ""						<< std::endl; // setFileLine(userDataFilePath, USER_STATUS, "");
		userDataOut << ""						<< std::endl; // setFileLine(userDataFilePath, CURRENT_RECIPIENT, "");

		std::cout << "User account " << shortenHash(requestData.loginHash) << " created." << std::endl;
		std::cout << "User " << shortenHash(requestData.loginHash) << " logged in." << std::endl;

		userDataOut.close();

		return "OK";
	}
}

std::string loginUser(DataPacket requestData)
{
	std::string userDataFilePath = "data/" + requestData.loginHash + "/user.dat";
	std::fstream userData(userDataFilePath);

	if (userData.fail())
	{
		return "No such user found.\nPlease, check login for mistakes and try again.";
	}

	int attemptsLeft = std::stoi(getFileLine(userData, LOGIN_ATTEMPTS_LEFT));

	if (requestData.passwordHash == getFileLine(userData, PASSWORD_HASH) && attemptsLeft > 0)
	{
		std::string lastLoginTime = getFileLine(userData, LAST_LOGIN_TIME);

		setFileLine(userDataFilePath, LAST_LOGIN_TIME, requestData.time);
		setFileLine(userDataFilePath, LOGIN_ATTEMPTS_LEFT, "3");
		setFileLine(userDataFilePath, ONLINE_STATUS, "online");

		std::cout << "User " << shortenHash(requestData.loginHash) << " logged in." << std::endl;

		return lastLoginTime;
	}
	else
	{
		if (attemptsLeft > 0)
		{
			setFileLine(userDataFilePath, LOGIN_ATTEMPTS_LEFT, std::to_string(--attemptsLeft));

			if (attemptsLeft == 0)
			{
				std::cout << "User account " << shortenHash(requestData.loginHash) << " suspended." << std::endl;

				return "Account has been suspended.\nPlease, contact an administrator.";
			}

			return "Incorrect password. Attempts left: " + std::to_string(attemptsLeft);
		}
		else
		{
			return "Account has been suspended.\nPlease, contact an administrator.";
		}
	}
}

std::string setUserStatus(DataPacket requestData)
{
	std::string userDataFilePath = "data/" + requestData.loginHash + "/user.dat";
	std::fstream userData(userDataFilePath);

	if (requestData.passwordHash == getFileLine(userData, PASSWORD_HASH))
	{
		setFileLine(userDataFilePath, USER_STATUS, requestData.userStatus);

		std::cout << "User " << shortenHash(requestData.loginHash) << " set new status." << std::endl;

		return "OK";
	}
	else
	{
		return "Authorization error. Action blocked.";
	}
}

std::string connectRecipient(DataPacket requestData)
{
	if (opendir(("data/" + requestData.recipientHash).c_str()) == NULL)
	{
		return "No such user found.\nPlease, check username for mistakes and try again.";
	}
	else
	{
		std::string userDataFilePath = "data/" + requestData.loginHash + "/user.dat";
		std::string recipientDataFilePath = "data/" + requestData.recipientHash + "/user.dat";

		std::fstream userData(userDataFilePath);
		std::fstream recipientData(recipientDataFilePath);

		if (requestData.passwordHash == getFileLine(userData, PASSWORD_HASH))
		{
			std::fstream dialogWithUser("data/" + requestData.recipientHash + "/dialogs/" + requestData.loginHash, std::ios::out);
			std::fstream dialogWithRecipient("data/" + requestData.loginHash + "/dialogs/" + requestData.recipientHash, std::ios::out);

			setFileLine(userDataFilePath, CURRENT_RECIPIENT, requestData.recipientHash);

			std::cout << "User " << shortenHash(requestData.loginHash) << " connected to " << shortenHash(requestData.recipientHash) << "." << std::endl;

			std::string reply = getFileLine(recipientData, USER_STATUS);

			if (!isEmpty(reply))
			{
				reply = " ~ " + reply;
			}

			return reply + "\nCurrent status: " + getFileLine(recipientData, ONLINE_STATUS);
		}
		else
		{
			return "Authorization error. Action blocked.";
		}
	}
}

std::string disconnectRecipient(DataPacket requestData)
{
	std::string userDataFilePath = "data/" + requestData.loginHash + "/user.dat";
	std::fstream userData(userDataFilePath);

	if (requestData.passwordHash == getFileLine(userData, PASSWORD_HASH))
	{
		setFileLine(userDataFilePath, CURRENT_RECIPIENT, "");

		std::cout << "User " << shortenHash(requestData.loginHash) << " disconnected from " << shortenHash(requestData.recipientHash) << "." << std::endl;

		return "OK";
	}
	else
	{
		return "Authorization error. Action blocked.";
	}
}

std::string logoutUser(DataPacket requestData)
{
	std::string userDataFilePath = "data/" + requestData.loginHash + "/user.dat";
	std::fstream userData(userDataFilePath);

	if (requestData.passwordHash == getFileLine(userData, PASSWORD_HASH))
	{
		setFileLine(userDataFilePath, ONLINE_STATUS, "offline");

		std::cout << "User " << shortenHash(requestData.loginHash) << " logged out." << std::endl;

		return "OK";
	}
	else
	{
		return "Authorization error. Action blocked.";
	}
}

std::string deleteAccount(DataPacket requestData)
{
	std::string userDataFolder = "data/" + requestData.loginHash;
	std::fstream userData(userDataFolder + "/user.dat");

	if (requestData.passwordHash == getFileLine(userData, PASSWORD_HASH))
	{
		sysbash("rm -r", userDataFolder);

		std::cout << "User " << shortenHash(requestData.loginHash) << " logged out." << std::endl;
		std::cout << "User account " << shortenHash(requestData.loginHash) << " deleted." << std::endl;

		return "OK";
	}
	else
	{
		return "Authorization error. Action blocked.";
	}
}

#endif 
