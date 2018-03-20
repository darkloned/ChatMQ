#include "ServerUtils.h"

void sysbash(std::string cmd, std::string filepath)
{
	std::system((cmd + " " + filepath).c_str());
}

void setFileLine(std::string filepath, int number, std::string line)
{
    // C++ is not able to laconically handle such easy thing as rewriting one line in the file...
    // Gosh, is there anything that can be written fast without using third-party libraries?! Duh.
	sysbash("sed -i \'" + std::to_string(number) + "s/.*/\'\"" + line + "\"\'/\'", filepath);
}

std::string getFileLine(std::fstream& file, int number)
{
    file.seekg(std::ios::beg);

    for(int i = 0; i < number - 1; ++i)
    {
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::string line;
    std::getline(file, line);

    return line;
} 
