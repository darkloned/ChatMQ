#include "ClientUtils.h"

// username must contain only [a-z, A-Z, 0-9]
bool incorrectUsername(std::string username)
{
	for (uint i = 0; i < username.length(); ++i)
	{
		if (!std::isdigit(username[i]) && !std::isalpha(username[i]))
		{
			return true;
		}
	}

	return false;
}

// for Unix-like OS
int getch()
{
    int c;
    struct termios t_old, t_new;

    tcgetattr(STDIN_FILENO, &t_old);

    t_new = t_old;
    t_new.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

    c = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &t_old);

    return c;
}

void getpass(std::string& password)
{
	const char BACKSPACE = 127;
	const char ENTER = 10;

	unsigned char c = 0;

	while ((c = getch()) != ENTER)
	{
		if (c == BACKSPACE)
		{
			if (password.length() != 0)
			{
				 std::cout << "\b \b";

				 password.resize(password.length() - 1);
			}
		}
		else
		{
			password += c;
			
			std::cout << '*';
		}
	}

	std::cout << std::endl;
}

std::string getCurrentTime()
{
	auto chronoTime = std::chrono::system_clock::now();
	std::time_t currentTime = std::chrono::system_clock::to_time_t(chronoTime);
	std::stringstream ss;

	ss << std::put_time(std::localtime(&currentTime), "%d-%m-%Y at %H:%M");

	return ss.str();
} 
