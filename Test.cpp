#include <iostream>
#include <Windows.h>
#include <wincred.h>
#include "resource.h"
#include "sms_dll.h"

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

		TerminateProcess(ProgramProcessHandle[i], 69);

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

			if (!CreateDirectory(UserFolderLocation.c_str(), NULL)) {

				printMsg("\n\t\tCreating User Folder Failed\t\t\n");

				this->error = true;
				return;

			}

		}

		SetCurrentDirectory(UserFolderLocation.c_str());

		if (!bFileExists("DataBase")) {

			if (!CreateDirectory("DataBase", NULL)) {

				printMsg("\n\t\tCreating DataBase Failed\t\t\n");

				this->error = true;
				return;
			}

		}
		
		SetCurrentDirectory(this->CurrentDirectory.c_str());

		if (!bFileExists("DataBase")) {

			if (!CreateDirectory("DataBase", NULL)) {

				printMsg("\n\t\tCreating DataBase Failed\t\t\n");

				this->error = true;
				return;
			}

		}

		if (!bFileExists("DataBase\\Scripts")) {

			if (!CreateDirectory("DataBase\\Scripts", NULL)) {

				printMsg("\n\t\tCreating Script Folder Failed\t\t\n");

				this->error = true;
				return;
			}

		}


	}

	void MoveCredentialsFiles() {
		
		std::string UserFolderLocation = "C:\\Users\\" + this->UserName + "\\Documents\\School Management System";
		std::string UserFileLocation = UserFolderLocation + "\\DataBase\\Credentials.txt";
		
		if (!CopyFile("DataBase\\Credentials.txt", UserFileLocation.c_str(), false)) {

			printMsg("\n\t\tCopying Credentials File Failed\t\t\n" + std::to_string(GetLastError()));

		}

	}

	void EncrpytCredentialsFile() {

		std::string UserFolderLocation = "C:\\Users\\" + this->UserName + "\\Documents\\School Management System";
		std::string UserFileLocation = UserFolderLocation + "\\DataBase\\Credentials.txt";

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

	//Privilege & AskForAdminRights() will be worked on if needed

	PRIVILEGE_SET privilegeSet = {};
	privilegeSet.Control = PRIVILEGE_SET_ALL_NECESSARY;

	BOOL PriviledgeStatus = true;

	PrivilegeCheck(dwProcessHandle, &privilegeSet,&PriviledgeStatus);

	if(!PriviledgeStatus){

		AskForAdminRights();

	}
	
	//Enviroment(Directories) Begun Creation

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

	env->MoveCredentialsFiles();

	delete(env);
	
	delete(cred);


	return true;
}


bool CheckScripts() {

	//Change with Number Scripts Addition
	std::string ScriptLocation = "DataBase\\Scripts";

	std::string ScriptNames[2] = { "MainScript.exe","OcrScript.exe" };


	for (int i = 0; i < sizeof(ScriptNames)/sizeof(std::string); i++) {

		if (!bFileExists(ScriptLocation + ScriptNames[i])) {

			return false;

		}

	}

	return true;

}


bool InitializeScripts() {

	if (!CheckScripts()) {

		printMsg("\n\tScripts Donot Exist\t\n");

		return false;

	}

}


int main()
{

	InitializeProgram();

	InitializeScripts();



	return ERROR_SUCCESS | GetLastError();

}