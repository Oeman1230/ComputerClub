
#include <iostream>
#include <string>

#include "WorkTime.h"
#include "ComputerClub.h"

int main(int argc, char* argv[])
{

	if (argc < 1 || argv[1] == NULL)
	{
		return -1;
	}

	std::string programStartArg = argv[1];
	//процедура запуска приложения
	ComputerCub club = ComputerCub(programStartArg);
	club.start();

	return 0;
}

