// CreateProcess.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>
#include <string>
#include <TlHelp32.h>

bool CreateProcessFunction();
bool TaskListFunction();

int _tmain(int argc, _TCHAR* argv[])
{
	CreateProcessFunction();
	TaskListFunction();

	getchar();
	return 0;
}

bool TaskListFunction()
{
	HANDLE handleProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if(handleProcessSnapshot == INVALID_HANDLE_VALUE)
		return false;

	PROCESSENTRY32 processEntry32;
	processEntry32.dwSize = sizeof(PROCESSENTRY32);

	if(!Process32First(handleProcessSnapshot, &processEntry32))
	{
		printf_s("Process 32 First ERROR. \n");
		CloseHandle(handleProcessSnapshot);

		return false;
	}

	do 
	{
		_tprintf_s(L"%d : %s\n", processEntry32.th32ProcessID, processEntry32.szExeFile);
	} while (Process32Next(handleProcessSnapshot, &processEntry32));

	CloseHandle(handleProcessSnapshot);

	return true;
}

bool CreateProcessFunction()
{
	STARTUPINFOA		startupInfo;
	PROCESS_INFORMATION	processInformation;

	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	ZeroMemory(&processInformation, sizeof(processInformation));

	std::string command = "notepad.exe";
	SetCurrentDirectoryA("C:\\Windows");

	// Start the Child Process.
	if(!CreateProcessA(command.c_str(), NULL, NULL, NULL, false, NULL, NULL, NULL, &startupInfo, &processInformation))
	{
		printf_s("Create Process Failed (%d) \n", GetLastError());
		return false;
	}

	printf_s("Create Complete.\n");
	WaitForSingleObject(processInformation.hProcess, INFINITE);

	printf_s("Parent Process Finished.\n");

	CloseHandle(processInformation.hProcess);
	CloseHandle(processInformation.hThread);

	return true;
}