#ifndef CLIENTUTILS_H
#define CLIENTUTILS_H

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <termios.h>
#include <unistd.h>

bool incorrectUsername(std::string username);

int getch();
void getpass(std::string& password);

std::string getCurrentTime();

#endif
