#include <iostream>
#include <Windows.h>
#include "sms_dll.h"
#include "resource.h"


std::vector<HANDLE>gProgramProcessHandle;


bool DlgInput();


void createCredentialsFile(std::string UserId, std::string UserKey) {

	std::fstream file("DataBase\\Credentials.txt", std::ios::out);

	file << "{\n";
	file << "UserId:" << cEncrypt(UserId)<<'\n';
	file << "UserKey:" << cEncrypt(UserKey);
	file << "\n}";
	
	file.close();

}


void TerminateProgram(std::vector<HANDLE>ProgramProcessHandle = gProgramProcessHandle) {

	for (int i = 0; i < ProgramProcessHandle.size(); i++) {

		TerminateProcess(ProgramProcessHandle[i], 0);

	}

}


INT_PTR CALLBACK DlgProcCredentialsInput(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	WORD dUserIdLen = 0;
	WORD dUserKeyLen = 0;


	char cUserId[50], cUserKey[50];
	std::string sUserId = "", sUserKey = "";


	switch (msg) {

	case WM_INITDIALOG:
		return 0;


	case WM_COMMAND:

		switch (LOWORD(wParam)) {
		case IDOK:


			dUserIdLen = (WORD)SendDlgItemMessageA(hWnd, IDC_USERID, EM_LINELENGTH, wParam, lParam);
			dUserKeyLen = (WORD)SendDlgItemMessageA(hWnd, IDC_USERKEY, EM_LINELENGTH, wParam, lParam);


			if (dUserIdLen < 3 || dUserKeyLen < 3) {
				SendMessage(hWnd, WM_QUIT, wParam, lParam);
				DlgInput();
			}


			*((LPWORD)cUserId) = dUserIdLen;
			*((LPWORD)cUserKey) = dUserKeyLen;


			cUserId[dUserIdLen] = '\0';
			cUserKey[dUserKeyLen] = '\0';


			SendDlgItemMessageA(hWnd, IDC_USERID, EM_GETLINE, (WPARAM)0, (LPARAM)cUserId);
			SendDlgItemMessageA(hWnd, IDC_USERKEY, EM_GETLINE, (WPARAM)0, (LPARAM)cUserKey);


			sUserId = cUserId;
			sUserKey = cUserKey;


			if (sUserId != "" && sUserKey != "") {
				createCredentialsFile(sUserId, sUserKey);
			}


			SendMessage(hWnd, WM_QUIT, wParam, lParam);
			return 0;
		}
		return 0;

	case WM_CLOSE:

		if (MessageBox(hWnd, "\tThe Progam Will Not Run Without Authentication\n\n\t\tDo You Want To Close The Program\t", "Program Terminating", MB_YESNO) == IDYES) {

			SendMessage(hWnd, WM_QUIT, wParam, lParam);
		}

		return 0;

	case WM_QUIT:

		DestroyWindow(hWnd);
		return 0;

	default:

		return 0;

	}

	return DefWindowProcA(hWnd, msg, wParam, lParam);

}


bool DlgInput() {

	HWND hWnd = {};
	hWnd = CreateDialog(NULL, MAKEINTRESOURCE(IDD_INITBOX), NULL, DlgProcCredentialsInput);


	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);


	MSG msg = {};


	while (GetMessageW(&msg, hWnd, NULL, NULL) > 0) {

		TranslateMessage(&msg);
		DispatchMessageA(&msg);

	}

	return true;

}


class Enviroment {

public:

	std::string CurrentDirectory = "";
	std::string UserName = "";

	bool error = false;

	Enviroment() {

		char* UserName = (char*)malloc(25);

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

	void CreateEnviromentFolders() {

		std::string UserFolderLocation = "C:\\Users\\" + this->UserName + "\\Documents\\School Management System";

		if (!bFileExists(UserFolderLocation)) {

			if (CreateDirectory(UserFolderLocation.c_str(), NULL)) {

				printMsg("\n\t\tCreating User Folder Failed\t\t\n");

				this->error = true;
				return;

			}

		}

		std::string UserFileLocation = UserFolderLocation + "\\DataBase";

		if (!bFileExists(UserFileLocation)) {

			if (CreateDirectory(UserFolderLocation.c_str(), NULL)) {

				printMsg("\n\t\tCreating User DataBase Failed\t\t\n");

				this->error = true;
				return;

			}

		}

		if (!bFileExists("DataBase")) {

			if (CreateDirectory("DataBase", NULL)) {

				printMsg("\n\t\tCreating DataBase Failed\t\t\n");

				this->error = true;
				return;
			}

		}

	}

	void MoveCredentialsFiles() {
		
		std::string UserFolderLocation = "C:\\Users\\" + this->UserName + "\\Documents\\School\x0020Management\x0020System";
		std::string UserFileLocation = UserFolderLocation + "\\DataBase\\Credentials.txt";
		
		if (!MoveFile("DataBase\\Credentials.txt", UserFileLocation.c_str())) {

			printMsg("\n\t\tMoving Credentials File Failed\t\t\n");

		}

	}

	void FinalKey() {

		std::string UserFolderLocation = "C:\\Users\\" + this->UserName + "Documents\\School\x0020Management\x0020System";
		std::string UserFileLocation = UserFolderLocation + "\\DataBase\\Credentials.txt";

		EncryptFile(UserFileLocation.c_str());
		
		std::cin.get();
		
		DecryptFile(UserFileLocation.c_str(), 0);

	}

	~Enviroment() {



	}

};


class Credentials {
private:

	int Retry = 0;
	std::string UserId = "", UserKey = "", query = "";

public:

	bool Authenticated = false;
	
	Enviroment env;

	Credentials()
	{
		if (bFileExists("DataBase\\Credentials.txt")) {
		
			DlgInput();
		
			ReadCredentials();
		
		}
		else {

			ReadCredentials();
		
		}

	}

	void Authenticate() {


		Retry++;

		if (Retry > 2) {

			printMsg("\n\t\tProgram Failed To Authenticate\t\t\n");

			TerminateProgram();

		}

		std::string AuthResponse = "Valid User IdValid User Key";


		if (AuthResponse == cSendAuthRequest((TCHAR*)_T("data.niporeglobal.com"), (TCHAR*)_T(query.c_str()), 80)) {

			Authenticated = true;

			std::cout << "Authentication Successfull\n";

			return;
		
		}


		DlgInput();

		this->ReadCredentials();


		this->Authenticate();


	}

	void setUserId(std::string In) {

		this->UserId = In;

	}


	void setUserKey(std::string In) {

		this->UserKey = In;

	}
	
	
	void ReadCredentials() {

		std::fstream fobj("DataBase\\Credentials.txt", std::ios::in);

		std::string sFileData = "";

		std::string sTempFileLine;

		while (!fobj.eof()) {

			fobj >> sTempFileLine;
			sFileData = sFileData + sTempFileLine + '\n';

		}

		fobj.close();

		//sFile index 0 == UserId
		//sFile index 1 == UserKey
		//sFile index 2 == HarwareId

		std::string sFile[2] = { "", ""};

		int iCount = 0;

		for (int i = 0; i < (int)sFileData.size(); i++) {

			if (sFileData[i] == ':') {

				for (int j = 1; j < 65; j++) {
					sFile[iCount] += sFileData[i + j];
				}
				iCount++;

			}

		}

		this->UserId = sFile[0];
		this->UserKey = sFile[1];

		//sFile[0] is modified to store the query
		sFile[0] = "/test.php?userId=" + sFile[0];
		sFile[0] = sFile[0] + "&userKey=" + sFile[1];

		this->query = sFile[0];

	}


};


bool InitializeProgram() {

	//Main Process Added To Global ProgramProcessHandle

	DWORD dwProcessId = GetCurrentProcessId();

	HANDLE dwProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, false, dwProcessId);

	gProgramProcessHandle.push_back(dwProcessHandle);

	//Enviroment(Directories) Begun Creation !!! Still Buggy

	Enviroment* env = new Enviroment();
	
	env->CreateEnviromentFolders();

	//Read Or Input Credentials

	Credentials* cred = new Credentials();


	if (env->error) {

		printMsg("\n\t\tProgram Failed To Initialize\t\t\n");

		return false;

	}

	cred->Authenticate();

	if (!cred->Authenticated) {

		TerminateProgram();

	}

	delete(env);
	
	delete(cred);


	return true;
}


int main()
{

	InitializeProgram();

	return ERROR_SUCCESS | GetLastError();

}