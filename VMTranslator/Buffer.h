#pragma once
#include <fstream>
#include <cassert>
#include <cstring>

class Buffer
{
private:
	static const int TWIN_BUFF_SIZE = 50;
	std::ifstream inf;
	char* working;
	char* archived;

	int charTaken = 0;

	static void readFileBulk(std::ifstream& inf, char* target, int size)
	{
		int reads = 0;
		memset(target, 0, size);

		while (reads != size && inf)
			inf.get(target[reads++]);

		if (reads < size)
			target[reads] = '\0';
	}

public:
	int line_number = 1;
	int start_index = 0;
    std::string file_name;

	Buffer(const char* fileLoc) : inf{ fileLoc }
	{
		assert(inf);
		this->working = new char[TWIN_BUFF_SIZE] {};
		this->archived = new char[TWIN_BUFF_SIZE] {};

        file_name = std::string(fileLoc);
        file_name = file_name.substr(file_name.find_last_of("/\\") + 1);
        file_name = file_name.substr(0,file_name.find_last_of("/."));
	}

	const char& getChar(int index)
	{
		if (index == -1)
			index = start_index;

		if (charTaken == index)
		{
			std::swap(working, archived);
			readFileBulk(inf, working, TWIN_BUFF_SIZE);
			charTaken += TWIN_BUFF_SIZE;
		}

		if (charTaken - index <= TWIN_BUFF_SIZE)
		{
			index %= TWIN_BUFF_SIZE;
			return working[index];
		}

		index %= TWIN_BUFF_SIZE;
		return archived[index];
	}

	const char& getTopChar()
	{
		return getChar(start_index);
	}

	~Buffer()
	{
		delete[] working;
		delete[] archived;
	}
};