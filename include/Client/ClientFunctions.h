#ifndef CLIENT_H
#define CLIENT_H

#include "sha256.h"

#include "ClientUtils.h"
#include "DataPacket.h"

bool createAccount(DataPacket& requestData, std::string& username);
bool deleteAccount(DataPacket& requestData, std::string username);

bool loginUser(DataPacket& requestData, std::string& username);
bool logoutUser(DataPacket& requestData, std::string username, std::string passwordHash);

bool setUserStatus(DataPacket& requestData, std::string username, std::string passwordHash);

bool connectRecipient(DataPacket& requestData, std::string username, std::string passwordHash, std::string& recipient);
bool disconnectRecipient(DataPacket& requestData, std::string username, std::string passwordHash, std::string recipient);

void quitClientApp(DataPacket& requestData, std::string username, std::string passwordHash);

void printHelp();
void unknownRequest();

#endif
