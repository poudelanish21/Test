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

// The Fibonacci recurrence relation describes a sequence F
// where F(n) is { n = 0, a
//               { n = 1, b
//               { n > 1, F(n-2) + F(n-1)
// for some initial integral values a and b.
// If the sequence is initialized F(0) = 1, F(1) = 1,
// then this relation produces the well-known Fibonacci
// sequence: 1, 1, 2, 3, 5, 8, 13, 21, 34, ...

// Initialize a Fibonacci relation sequence
// such that F(0) = a, F(1) = b.
// This function must be called before any other function.

extern "C" SMS_DLL_API int iCheckConnection(TCHAR * url);

extern "C" SMS_DLL_API const char* cSendAuthRequest(TCHAR * url, TCHAR * query, INTERNET_PORT securityType);

extern "C" SMS_DLL_API char* iSendAuthRequest(TCHAR * url, TCHAR * query, INTERNET_PORT securityType);

extern "C" SMS_DLL_API bool bFileExists(std::string file_location);

extern "C" SMS_DLL_API int iGetFileSize(std::string file_location);

extern "C" SMS_DLL_API FILETIME FileCreatedDate(std::string file_location);

extern "C" SMS_DLL_API void printMsg(std::string msg);

extern "C" SMS_DLL_API char* cEncrypt(std::string in);
