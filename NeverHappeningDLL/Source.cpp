// NeverHappeningDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include "Source.h"
#include "tcpClient.h"
#include <stdio.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <vector>
#include <algorithm>
#include <string>
#include <tchar.h>

HANDLE DllThread;
TcpClient tcpClient;
DWORD pid = GetCurrentProcessId();

//int main(){
DWORD WINAPI Main(LPVOID lpParam)
{
	if (pid)
	{
		CheckModules(pid);
		ListProcessThreads(pid);
		postChecks();
	}
	else 
	{
		std::cout << "Game is not active." << std::endl;
	}

	CloseHandle(DllThread);
	return 0;
}

void postChecks() {
	while (true) {
		Sleep(900000);
		postDLLScan = true;
		postThreadScan = true;
		postDLL.clear();
		postThread.clear();
		ListProcessThreads(pid);
		CheckModules(pid);
		if (startingDLL.size() + deltaDLL.size() != postDLL.size()) {
			int newDLLs = postDLL.size() - startingDLL.size();
			for (int i = 0; i < newDLLs; i++) {
				tcpClient.start(postDLL[(startingDLL.size() - 1) + (newDLLs - i)] + " injected after the game has started.");
				deltaDLL.push_back(postDLL[(startingDLL.size() - 1) + (newDLLs - i)]);
			}
		}
		if (startingThread.size() + deltaThread.size() != postThread.size()) {
			int newThreads = postThread.size() - startingThread.size();
			for (int i = 0; i < newThreads; i++) {
				tcpClient.start(postThread[(startingThread.size() - 1) + (newThreads - i)] + " thread created after the game has started.");
				deltaThread.push_back(postThread[(startingThread.size() - 1) + (newThreads - i)]);
			}
		}
	}

}

//================================================================================================================
//CREATE MAIN THREAD Function
//================================================================================================================
void startThread() {
	//Start DLLMain Thread
	DllThread = CreateThread(0, 0, Main, 0, 0, 0);
}

//================================================================================================================
//DLLMAIN Function
//================================================================================================================
BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		//Create Main Thread
		startThread();
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{

	}

	return TRUE;
}

int CheckModules(DWORD processID)
{
	HMODULE hMods[1024];
	HANDLE hProcess;
	DWORD cbNeeded;
	unsigned int i;
	// Print the process identifier.

	printf("\nProcess ID: %u\n", processID);

	// Get a handle to the process.

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);
	if (NULL == hProcess)
		return 1;

	// Get a list of all the modules in this process.
	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
		{
			char szModName[MAX_PATH];
			// Get the full path to the module's file.

			if (GetModuleBaseNameA(hProcess, hMods[i], szModName,
				sizeof(szModName) / sizeof(TCHAR)))
			{
				// Print the module name and handle value.

				if (postDLLScan) {
					postDLL.push_back(szModName);
				}
				else {
					startingDLL.push_back(szModName);
				}

				// _tprintf(TEXT("\t%s\n"), szModName);
				// _tprintf(TEXT("\t%s (0x%08X)\n"), szModName, hMods[i]);
			}
		}
	}

	// Release the handle to the process.

	CloseHandle(hProcess);
	return 0;
}
BOOL ListProcessThreads(DWORD dwOwnerPID)
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	// Take a snapshot of all running threads  
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return(FALSE);

	// Fill in the size of the structure before using it. 
	te32.dwSize = sizeof(THREADENTRY32);

	// Retrieve information about the first thread,
	// and exit if unsuccessful
	if (!Thread32First(hThreadSnap, &te32))
	{
		printError(TEXT("Thread32First"));  // Show cause of failure
		CloseHandle(hThreadSnap);     // Must clean up the snapshot object!
		return(FALSE);
	}

	// Now walk the thread list of the system,
	// and display information about each thread
	// associated with the specified process
	do
	{
		if (te32.th32OwnerProcessID == dwOwnerPID)
		{
			char threadID[32];
			sprintf(threadID, "0x%du", te32.th32ThreadID);
			if (postThreadScan) {
				postThread.push_back(threadID);
			}
			else {
				startingThread.push_back(threadID);
			}
			// std::cout << threadID << std::endl;
			//_tprintf(TEXT("\n     base priority  = %d"), te32.tpBasePri);
			// _tprintf(TEXT("\n     delta priority = %d"), te32.tpDeltaPri);
		}
	} while (Thread32Next(hThreadSnap, &te32));

	// _tprintf(TEXT("\n"));

	//  Don't forget to clean up the snapshot object.
	CloseHandle(hThreadSnap);
	return(TRUE);
}
void printError(TCHAR* msg)
{
	DWORD eNum;
	TCHAR sysMsg[256];
	TCHAR* p;

	eNum = GetLastError();
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, eNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		sysMsg, 256, NULL);

	// Trim the end of the line and terminate it with a null
	p = sysMsg;
	while ((*p > 31) || (*p == 9))
		++p;
	do { *p-- = 0; } while ((p >= sysMsg) &&
		((*p == '.') || (*p < 33)));

	// Display the message
	_tprintf(TEXT("\n  WARNING: %s failed with error %d (%s)"), msg, eNum, sysMsg);
}

