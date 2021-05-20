#include <iostream>
#include <Windows.h>
#include "sms_dll.h"

class Enviroment {
public:

	std::string CurrentDirectory = "";
	std::string UserName = "";

	Enviroment() {

		char UserName[25];

		DWORD dwBuffer = 25;

		GetUserName(UserName, &dwBuffer);

		if (UserName == NULL) {

			printMsg("\n\t\tAllocating Memory Failure!!!\t\t\n");

			return;
		}

		this->UserName = UserName;

		char* CurrentDirectory = (char*)malloc(256);

		if (CurrentDirectory == 0) {

			printMsg("\n\t\tAllocating Memory Failure\t\t\n");

			return;
		}

		GetCurrentDirectory((DWORD)_msize(CurrentDirectory), CurrentDirectory);

		this->CurrentDirectory = CurrentDirectory;

		free(UserName);

		free(CurrentDirectory);

	}

};

int main()
{



	return ERROR_SUCCESS | GetLastError();

}