#include <iostream>
#include <string>
#include <Windows.h>
#include <wincred.h>
#include <tchar.h>
#include <fstream>
#include <websocket.h>
#include <thread>
#include <future>
#include "sms_dll.h"
#include "resource.h"

static std::vector<HANDLE>gProgramProcessHandle;

TCHAR* lpszClassName = (TCHAR*)_T("SMS_TEST_PROGRAM");


TCHAR* lpszApplicationName = (TCHAR*)_T("SMS_TEST_APPLICATION");


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	HWND hNotificationsWindow = NULL;

	switch (msg)
	{
	case WM_INITDIALOG:
	{

		return 0;

	}
	case WM_CREATE: {

		return 0;
	}
	case WM_SIZE:
	{

		return 0;

	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hWnd, &ps);

		return 0;

	}
	case WM_CLOSE:

		if (MessageBox(hWnd, _T("Have you saved changes ?"), _T("Save Changes"), MB_YESNO) == IDYES)

		{
			DestroyWindow(hWnd);
		}

		return 0;
	
	case WM_CHAR:
		
		switch (wParam) {
		
		//Esc
		case 27:
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;

		}
		return 0;
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;

	default:
		break;

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);

}


class WindowsWebSocket {

public:

	bool WebSocketActive = false;

	HRESULT hResult_server = {};
	WORD wVersionRequired = MAKEWORD(1, 1);
	WSADATA wsaData;
	int iErrorStatus = 0;
	SOCKET Socket = {};

	//Address	index = 0	HostIp
	//Address	index = 1	NetworkIp
	std::string sAddress[2] = { "","" };

	u_long ulAddress[2] = { 0,0 };

	u_short usPort[2] = { 0,0 };

	IN_ADDR in[2] = { 0,0 };

	std::string HostName = {};

	//IPPROTO_TCP = 6
	int Protocol = IPPROTO_TCP;

	WindowsWebSocket() {

		this->iErrorStatus = WSAStartup(wVersionRequired, &wsaData);

		if (this->iErrorStatus != 0) {

			printMsg("WinSock Initialization Failed");
			return;

		}

		if (LOBYTE(wVersionRequired) != LOBYTE(wsaData.wVersion)
			|| HIBYTE(wVersionRequired) != HIBYTE(wsaData.wVersion)) {

			printMsg("WinSock Implementation Failed");

			WSACleanup();
			return;

		}

		char* cHostName = (char*)malloc(50);

		if (cHostName == NULL) {

			printMsg("Memory Allocation Failed");
			return;
		}

		gethostname(cHostName, 50);

		this->HostName = cHostName;

		free(cHostName);

	}

	void SetHostIp(std::string Ip) {

		this->sAddress[0] = Ip;

		this->ulAddress[0] = inet_addr(Ip.c_str());

		memcpy(&this->in[0], &this->ulAddress[0], 4);

	}

	bool CreateServer() {

		//AF_INET = The Internet Protocol version 4 (IPv4) address family.
		// 
		//A socket type that provides sequenced, reliable, two-way, connection-based byte streams with an OOB data transmission mechanism.
		//This socket type uses the Transmission Control Protocol (TCP) for the Internet address family .

		//WSASocket();
		this->Socket = socket(AF_INET, SOCK_STREAM, Protocol);

		if (this->Socket == INVALID_SOCKET) {

			printMsg("Socket Creation Failed");

			return false;

		}

		return true;

	}


	void SetNetworkIp(std::string Ip) {

		this->sAddress[1] = Ip;

		this->ulAddress[1] = inet_addr(Ip.c_str());

		memcpy(&this->in[1], &this->ulAddress[1], 4);

	}


	~WindowsWebSocket() {

		if (WSAGetLastError() != 0) {

			printMsg("Error Found With Code: " + std::to_string(WSAGetLastError()));

		}

		WSACleanup();

	}

};


ATOM Register(HINSTANCE hInstance) {

	WNDCLASS wcex;

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hIcon = NULL;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = lpszClassName;

	return RegisterClass(&wcex);

}


bool InitInstance(HINSTANCE hInstance, int iCmdShow) {

	HWND hWnd = {};

	hWnd = CreateWindow(
		lpszClassName,
		lpszApplicationName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0,
		NULL, NULL, hInstance, NULL);

	if (hWnd == NULL) {

		return FALSE;

	}

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	return TRUE;

}


int MainWindow() {

	//GetModuleHandle(NULL) returns handle of the current program
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);



	int nCmdShow = SW_SHOW;

	if (!Register(hInstance)) {

		return GetLastError();

	}

	if (!InitInstance(hInstance, nCmdShow)) {

		return GetLastError();

	}

	HACCEL hAccel = {};
	//Loading of the accelator is on hold

	MSG msg = { };

	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{

		if (msg.wParam == WM_QUIT) {

			return 0;

		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}

	return (int)msg.wParam;

}


int InitializeDevice() {

	//Run Window On Another Thread
	std::thread MainWindowThread = std::thread(MainWindow);


	HANDLE hThreadHandle = MainWindowThread.native_handle();


	gProgramProcessHandle.push_back(hThreadHandle);


	if (MainWindowThread.joinable()) {

		MainWindowThread.join();

	}

	return 0;

}


bool DlgInput();


void createCredentialsFile(std::string UserId, std::string UserKey) {

	std::fstream file("DataBase\\Credentials.txt", std::ios::out);

	file << "{\n";
	file << "UserId:" << cEncrypt(UserId) << '\n';
	file << "UserKey:" << cEncrypt(UserKey);
	file << "\n}";

	file.close();

}


INT_PTR CALLBACK DlgProcCredentialsInput(HWND hWnd, 
	UINT msg, WPARAM wParam, LPARAM lParam)
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


void TerminateProgram(
	std::vector<HANDLE>ProgramProcessHandle = gProgramProcessHandle
) {

	for (int i = 0; i < ProgramProcessHandle.size(); i++) {

		TerminateProcess(ProgramProcessHandle[i], 69);

	}

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

		std::string sFile[2] = { "", "" };

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

	PrivilegeCheck(dwProcessHandle, &privilegeSet, &PriviledgeStatus);

	if (!PriviledgeStatus) {

		//AskForAdminRights() Is Not Ready To DEploy
		//AskForAdminRights();

	}

	CloseHandle(dwProcessHandle);

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


	for (int i = 0; i < sizeof(ScriptNames) / sizeof(std::string); i++) {

		if (!bFileExists(ScriptLocation + ScriptNames[i])) {

			return false;

		}

	}

	return true;

}


void Download(std::string URL, std::string FileName, std::string* Response) {

	*Response = cURLDownloadToFile(URL, FileName);

}

bool DownloadScripts() {

	const int Count = 1;

	std::string FileName[Count] = {""};
	std::string URL[Count] = {""};
	std::string Response[Count] = {""};
	std::thread DownloadThread[Count] = {};

	for (int index = 0; index < Count; index++) {

		if (!bFileExists("DataBase\\Scripts" + FileName[index])) {

			//MultiThread Approach
			DownloadThread[index] = std::thread(Download, URL[index], FileName[index], &Response[index]);

			HANDLE hThreadHandle = DownloadThread[index].native_handle();

			gProgramProcessHandle.push_back(hThreadHandle);


			//Single Thread Approach
			/*if (cURLDownloadToFile(URL[index], FileName[index]) != (char*)"Sucess\0") {

				printMsg("\n\tError While Downloading " + FileName[index] + "\t\n");

				return false;
			}*/

		}

	}

	for (int index = 0; index < Count; index++) {

		if (DownloadThread[index].joinable()) {

			DownloadThread[index].join();

		}

	}


	for (int index = 0; index < Count; index++) {

		if (Response[index] != "Success\0") {

			printMsg("\n\tError While Downloading " + FileName[index] + "\t\n");

		}

	}

	return true;

}


bool InitializeScripts() {

	if (!CheckScripts()) {

		//printMsg("\n\tScripts Does not Exist\t\n");

		//Run in different thread

		DownloadScripts();

		//printMsg("\n\tScripts Download Started\t\n");

		return false;

	}

	return true;

}


int main()
{

	InitializeProgram();

	InitializeScripts();

	InitializeDevice();

	if (GetLastError()) {
	


	};

}