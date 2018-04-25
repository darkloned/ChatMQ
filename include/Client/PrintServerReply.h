#ifndef PRINTSERVERREPLY_H
#define PRINTSERVERREPLY_H

#include <iostream>

#include "DataPacket.h"

void replyCreatingAccount(std::string reply, DataPacket requestData, std::string& username, std::string& passwordHash);
void replyDeletingAccount(std::string reply, std::string& username, std::string& passwordHash, std::string& recipient);

void replyLoggingUserIn(std::string reply, DataPacket requestData, std::string& username, std::string& passwordHash);
void replyLoggingUserOut(std::string reply, std::string& username, std::string& passwordHash, std::string& recipient);

void replySettingUserStatus(std::string reply);

void replyConnectingRecipient(std::string reply, std::string& recipient);
void replyDisconnectingRecipient(std::string reply, std::string& recipient);

void replyQuittingClientApp(std::string reply, std::string username);

#endif 
