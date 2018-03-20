#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <iostream>

bool isEmpty(std::string s);
bool contains(std::string str, std::string substr);
void trim(std::string& s);

std::string shortenHash(std::string loginHash);

#endif
