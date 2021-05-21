#pragma once
#include <algorithm>
#include <cassert>
#include <iterator>
#include <sstream>
#include <vector>
#include <fstream>
#include <utility>
#include <limits.h>
#include <Windows.h>
#include <wininet.h>
#include <urlmon.h>
#include <string>
#include <tchar.h>
#include <iostream>

#ifdef SMS_DLL_EXPORTS
#define sms_dll_API __declspec(dllexport)
#else
#define SMS_DLL_API __declspec(dllimport)
#endif

extern "C" SMS_DLL_API int iCheckConnection(TCHAR * url);

extern "C" SMS_DLL_API const char* cSendAuthRequest(TCHAR * url, TCHAR * query, INTERNET_PORT securityType);

extern "C" SMS_DLL_API char* iSendAuthRequest(TCHAR * url, TCHAR * query, INTERNET_PORT securityType);

extern "C" SMS_DLL_API bool bFileExists(std::string file_location);

extern "C" SMS_DLL_API int iGetFileSize(std::string file_location);

extern "C" SMS_DLL_API FILETIME FileCreatedDate(std::string file_location);

extern "C" SMS_DLL_API void printMsg(std::string msg);

extern "C" SMS_DLL_API char* cEncrypt(std::string in);

extern "C" SMS_DLL_API bool AskForAdminRights();