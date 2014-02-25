// Thread1.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>

#define MAX_THREAD	10

DWORD_PTR Affinity_Mask = 0x0003;

unsigned int WINAPI ThreadProc(LPVOID lpParam)
{
	if(0 == SetThreadAffinityMask(GetCurrentThread(), Affinity_Mask))
		printf_s("Affinity fail : %d \n", GetLastError());

	SetThreadPriority(GetCurrentThread(), *((int* )lpParam));

	for(int a = 0; a < 100; ++a)
	{
		for(int i = 0; i < 10000000; ++i)
		{
			; // busy waiting...
		}
		Sleep(0);
	}

	printf_s("Thread Priority %d END \n", *((int* )lpParam));

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD	dwThreadId[MAX_THREAD]		= {0, };
	HANDLE	hThread[MAX_THREAD]			= {NULL, };
	int		threadPriority[MAX_THREAD]	= {0, };

	for (int i = 0; i < MAX_THREAD; ++i)
	{
		switch (i % 5)
		{
		case 0:
			threadPriority[i] = THREAD_PRIORITY_IDLE;
			break;
		case 1:
			threadPriority[i] = THREAD_PRIORITY_LOWEST;
			break;
		case 2:
			threadPriority[i] = THREAD_PRIORITY_NORMAL;
			break;
		case 3:
			threadPriority[i] = THREAD_PRIORITY_HIGHEST;
			break;
		case 4:
			threadPriority[i] = THREAD_PRIORITY_TIME_CRITICAL;
			break;
		default:
			break;
		}
	}

	for (int i = 0; i < MAX_THREAD; ++i)
	{
		hThread[i] = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, (LPVOID)&threadPriority[i], 0, (unsigned *)&dwThreadId[i]);
		
		if(hThread[i] == NULL)
			return -1;
	}

	WaitForMultipleObjects(MAX_THREAD, hThread, TRUE, INFINITE);

	for(int i = 0; i < MAX_THREAD; ++i)
	{
		CloseHandle(hThread[i]);
	}

	getchar();

	return 0;
}

