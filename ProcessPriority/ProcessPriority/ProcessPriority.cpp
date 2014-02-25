// ProcessPriority.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "windows.h"
#include <string>

#define MAX_CHILD	12

int _tmain(int argc, _TCHAR* argv[])
{
	STARTUPINFOA		startupInformation[MAX_CHILD];
	PROCESS_INFORMATION	processInformation[MAX_CHILD];
	HANDLE				handles[MAX_CHILD] = {NULL, };

	ZeroMemory(&startupInformation, sizeof(startupInformation));
	ZeroMemory(&processInformation, sizeof(processInformation));

	char	command[128];

	for (int i = 0; i < MAX_CHILD; ++i)
	{
		startupInformation[i].cb = sizeof(startupInformation[i]);
		memset(command, NULL, sizeof(command));

		sprintf_s(command, "BusyWorker.exe %d", i);

		if(!CreateProcessA(NULL, command, NULL, NULL, false, NULL, NULL, NULL, &startupInformation[i], &processInformation[i]))
		{
			printf_s("Create Process failed (%d) \n", GetLastError());
			return 0;
		}

		handles[i] = processInformation[i].hProcess;
	}

	WaitForMultipleObjects(MAX_CHILD, handles, true, INFINITE);

	for(int i = 0; i < MAX_CHILD; ++i)
	{
		CloseHandle(processInformation[i].hProcess);
		CloseHandle(processInformation[i].hThread);
	}

	return 0;
}

