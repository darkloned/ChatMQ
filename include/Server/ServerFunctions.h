#ifndef SERVER_H
#define SERVER_H

#include <memory.h>
#include <dirent.h>
#include <sys/stat.h>

#include "ServerUtils.h"
#include "DataPacket.h"

std::string createAccount(DataPacket requestData);
std::string deleteAccount(DataPacket requestData);

std::string loginUser(DataPacket requestData);
std::string logoutUser(DataPacket requestData);

std::string setUserStatus(DataPacket requestData);

std::string connectRecipient(DataPacket requestData);
std::string disconnectRecipient(DataPacket requestData);

#endif 
