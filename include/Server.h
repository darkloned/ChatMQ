#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cstring>

#define RWXUSR S_IRUSR | S_IWUSR | S_IXUSR

std::string createNewAccount(std::string loginHash, std::string passwordHash)
{
	if (mkdir(("data/" + loginHash).c_str(), RWXUSR) == -1) // user data
	{
		if (errno == EEXIST)
		{
			return "User with such login already exists.\nPlease, choose another one.";
		}

		return "Error occured while creating account.";
	}
	else
	{
		std::ofstream credout;
		std::string userDataFilePath = "data/" + loginHash + "/user.dat";

		credout.open(userDataFilePath, std::ios::out | std::ios::app); // account details

		credout << passwordHash << std::endl;
		std::cout << "New user @{" << loginHash.substr(0, 6) << "} joined and logged in." << std::endl;

		credout.close();

		return "OK";
	}
}

std::string authorize(std::string loginHash, std::string passwordHash)
{

}


#endif 
