#include "StringUtils.h"

bool isEmpty(std::string s)
{
	return s.size() == 0;
}

bool contains(std::string str, std::string substr)
{
	return str.find(substr) != std::string::npos;
}

void trim(std::string& s)
{
	const std::string INVISIBLE = " \f\n\r\t\v";
	std::string sRightTrimmed = s.erase(s.find_last_not_of(INVISIBLE) + 1);

	s = sRightTrimmed.erase(0, sRightTrimmed.find_first_not_of(INVISIBLE));
}

std::string shortenHash(std::string loginHash)
{
	return "@{" + loginHash.substr(0, 6) + "}" ;
}
