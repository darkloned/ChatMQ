#include "ServerFunctions.h"

std::string createAccount(DataPacket requestData)
{
	if (mkdir(("data/" + requestData.usernameHash).c_str(), RWXUSR) == -1)
	{
		if (errno == EEXIST)
		{
			return "User with such login already exists.\nPlease, choose another one.";
		}

		return "Error has occured while creating account.";
	}
	else
	{
		mkdir(("data/" + requestData.usernameHash + "/dialogs").c_str(), RWXUSR);

		std::ofstream userDataOut;
		std::string userDataFilePath = "data/" + requestData.usernameHash + "/user.dat";

		userDataOut.open(userDataFilePath, std::ios::out | std::ios::app);

		userDataOut << requestData.passwordHash << std::endl; // setFileLine(userDataFilePath, PASSWORD_HASH, requestData.passwordHash);
		userDataOut << requestData.time 		<< std::endl; // setFileLine(userDataFilePath, LAST_LOGIN_TIME, requestData.time);
		userDataOut << "3" 						<< std::endl; // setFileLine(userDataFilePath, LOGIN_ATTEMPTS_LEFT, "3");
		userDataOut << "online"					<< std::endl; // setFileLine(userDataFilePath, LOGIN_STATUS, "online");
		userDataOut << ""						<< std::endl; // setFileLine(userDataFilePath, USER_STATUS, "");
		userDataOut << ""						<< std::endl; // setFileLine(userDataFilePath, CURRENT_RECIPIENT, "");

		std::cout << "User account " << shortenHash(requestData.usernameHash) << " created." << std::endl;
		std::cout << "User " << shortenHash(requestData.usernameHash) << " logged in." << std::endl;

		userDataOut.close();

		return "OK";
	}
}

std::string loginUser(DataPacket requestData)
{
	std::string userDataFilePath = "data/" + requestData.usernameHash + "/user.dat";
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

		std::cout << "User " << shortenHash(requestData.usernameHash) << " logged in." << std::endl;

		return lastLoginTime;
	}
	else
	{
		if (attemptsLeft > 0)
		{
			setFileLine(userDataFilePath, LOGIN_ATTEMPTS_LEFT, std::to_string(--attemptsLeft));

			if (attemptsLeft == 0)
			{
				std::cout << "User account " << shortenHash(requestData.usernameHash) << " suspended." << std::endl;

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
	std::string userDataFilePath = "data/" + requestData.usernameHash + "/user.dat";
	std::fstream userData(userDataFilePath);

	if (requestData.passwordHash == getFileLine(userData, PASSWORD_HASH))
	{
		setFileLine(userDataFilePath, USER_STATUS, requestData.userStatus);

		std::cout << "User " << shortenHash(requestData.usernameHash) << " set new status." << std::endl;

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
		std::string userDataFilePath = "data/" + requestData.usernameHash + "/user.dat";
		std::string recipientDataFilePath = "data/" + requestData.recipientHash + "/user.dat";

		std::fstream userData(userDataFilePath);
		std::fstream recipientData(recipientDataFilePath);

		if (requestData.passwordHash == getFileLine(userData, PASSWORD_HASH))
		{
			std::fstream dialogWithUser("data/" + requestData.recipientHash + "/dialogs/" + requestData.usernameHash);
			//std::fstream dialogWithRecipient("data/" + requestData.usernameHash + "/dialogs/" + requestData.recipientHash);

			setFileLine(userDataFilePath, CURRENT_RECIPIENT, requestData.recipientHash);

			std::cout << "User " << shortenHash(requestData.usernameHash) << " connected to " << shortenHash(requestData.recipientHash) << "." << std::endl;

			std::string reply = getFileLine(recipientData, USER_STATUS);

			if (!isEmpty(reply))
			{
				reply = " ~ " + reply;
			}

			reply += "\nCurrent status: " + getFileLine(recipientData, ONLINE_STATUS) + '\n';
			reply += "Last conversation lines:";

			std::ifstream dialog("data/" + requestData.recipientHash + "/dialogs/" + requestData.usernameHash);

			int wcl = std::count(std::istreambuf_iterator<char>(dialog), std::istreambuf_iterator<char>(), '\n');

			for (int i = std::max(1, wcl - 10); i <= wcl; ++i)
			{
				reply += '\n' + getFileLine(dialogWithUser, i);
			}

			return reply;
		}
		else
		{
			return "Authorization error. Action blocked.";
		}
	}
}

std::string pushMessage(DataPacket requestData, std::vector<DataPacket>& messageStock)
{
	std::string userDataFilePath = "data/" + requestData.usernameHash + "/user.dat";

	std::fstream userData(userDataFilePath);

	if (requestData.passwordHash == getFileLine(userData, PASSWORD_HASH))
	{
		messageStock.push_back(requestData);

		std::system(("echo \"" + requestData.msg + "\" >> data/" + requestData.usernameHash + "/dialogs/" + requestData.recipientHash).c_str());
		std::system(("echo \"" + requestData.msg + "\" >> data/" + requestData.recipientHash + "/dialogs/" + requestData.usernameHash).c_str());

		return "OK";
	}
	else
	{
		return "Authorization error. Action blocked.";
	}
}

std::string popMessage(DataPacket requestData, std::vector<DataPacket>& messageStock)
{
	std::string msg = "";

	for (uint i = 0; i < messageStock.size(); ++i)
	{
		if (messageStock[i].recipientHash == requestData.usernameHash)
		{
			msg += messageStock[i].msg + "\n";

			messageStock.erase(messageStock.begin() + i);

			i--;
		}
	}

	return msg;
}

std::string disconnectRecipient(DataPacket requestData)
{
	std::string userDataFilePath = "data/" + requestData.usernameHash + "/user.dat";
	std::fstream userData(userDataFilePath);

	if (requestData.passwordHash == getFileLine(userData, PASSWORD_HASH))
	{
		setFileLine(userDataFilePath, CURRENT_RECIPIENT, "");

		std::cout << "User " << shortenHash(requestData.usernameHash) << " disconnected from " << shortenHash(requestData.recipientHash) << "." << std::endl;

		return "OK";
	}
	else
	{
		return "Authorization error. Action blocked.";
	}
}

std::string logoutUser(DataPacket requestData)
{
	std::string userDataFilePath = "data/" + requestData.usernameHash + "/user.dat";
	std::fstream userData(userDataFilePath);

	if (requestData.passwordHash == getFileLine(userData, PASSWORD_HASH))
	{
		setFileLine(userDataFilePath, ONLINE_STATUS, "offline");
		setFileLine(userDataFilePath, CURRENT_RECIPIENT, "");

		std::cout << "User " << shortenHash(requestData.usernameHash) << " logged out." << std::endl;

		return "OK";
	}
	else
	{
		return "Authorization error. Action blocked.";
	}
}

std::string deleteAccount(DataPacket requestData)
{
	std::string userDataFolder = "data/" + requestData.usernameHash;
	std::fstream userData(userDataFolder + "/user.dat");

	if (requestData.passwordHash == getFileLine(userData, PASSWORD_HASH))
	{
		sysbash("rm -r", userDataFolder);

		std::cout << "User " << shortenHash(requestData.usernameHash) << " logged out." << std::endl;
		std::cout << "User account " << shortenHash(requestData.usernameHash) << " deleted." << std::endl;

		return "OK";
	}
	else
	{
		return "Authorization error. Action blocked.";
	}
} 
