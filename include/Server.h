#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <fstream>
#include <limits>
#include <cstring>
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

enum DataLine { PASSWORD_HASH = 1, LAST_LOGIN_TIME, LOGIN_ATTEMPTS_LEFT };

std::string createNewAccount(DataPacket requestData)
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
		std::ofstream userDataOut;
		std::string userDataFilePath = "data/" + requestData.loginHash + "/user.dat";

		userDataOut.open(userDataFilePath, std::ios::out | std::ios::app);

		userDataOut << requestData.passwordHash << std::endl; // setFileLine(userDataFilePath, PASSWORD_HASH, requestData.passwordHash);
		userDataOut << requestData.time 		<< std::endl; // setFileLine(userDataFilePath, LAST_LOGIN_TIME, requestData.time);
		userDataOut << "3" 						<< std::endl; // setFileLine(userDataFilePath, LOGIN_ATTEMPTS_LEFT, "3");

		std::cout << "New user @{" << requestData.loginHash.substr(0, 6) << "} joined and logged in." << std::endl;

		userDataOut.close();

		return "OK";
	}
}

std::string authorize(DataPacket requestData)
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

		std::cout << "User @{" << requestData.loginHash.substr(0, 6) << "} logged in." << std::endl;

		return lastLoginTime;
	}
	else
	{
		if (attemptsLeft > 0)
		{
			setFileLine(userDataFilePath, LOGIN_ATTEMPTS_LEFT, std::to_string(--attemptsLeft));

			if (attemptsLeft == 0)
			{
				std::cout << "User account @{" << requestData.loginHash.substr(0, 6) << "} suspended." << std::endl;

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

#endif 
