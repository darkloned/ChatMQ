#ifndef SERVERUTILS_H
#define SERVERUTILS_H

#include <iostream>
#include <fstream>
#include <limits>

#define RWXUSR S_IRUSR | S_IWUSR | S_IXUSR
 
enum DataLine { PASSWORD_HASH = 1,
                LAST_LOGIN_TIME, 
                LOGIN_ATTEMPTS_LEFT,
                ONLINE_STATUS,
                USER_STATUS,
                CURRENT_RECIPIENT };

void sysbash(std::string cmd, std::string filepath);

void setFileLine(std::string filepath, int number, std::string line);
std::string getFileLine(std::fstream& file, int number);

#endif
