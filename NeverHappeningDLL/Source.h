#pragma once
#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <vector>
#include <string>

#pragma once
#ifdef __cplusplus
int CheckModules(DWORD processID);
std::vector<std::string> startingDLL;
std::vector<std::string> postDLL;
std::vector<std::string> deltaDLL;
std::vector<std::string> startingThread;
std::vector<std::string> postThread;
std::vector<std::string> deltaThread;
bool postDLLScan;
bool postThreadScan;
void postChecks();
BOOL ListProcessThreads(DWORD dwOwnerPID);
void printError(TCHAR* msg);
std::wstring BFHeroes(L"PROCESSNAME");
#endif