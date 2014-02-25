// Thread3.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>

#define NUM_OF_THREAD	6

LONG gTotalCount = 0;

// �Ӱ� ���� ����ȭ�� ũ��Ƽ�� ����
CRITICAL_SECTION	hCriticalSection;

unsigned int WINAPI ThreadProc(LPVOID lpParam)
{
	for(DWORD i = 0; i < 10000; ++i)
	{
		EnterCriticalSection(&hCriticalSection);
		gTotalCount++;
		LeaveCriticalSection(&hCriticalSection);
	}

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD	dwThreadId[NUM_OF_THREAD];
	HANDLE	hThread[NUM_OF_THREAD];

	InitializeCriticalSection(&hCriticalSection);

	for(DWORD i = 0; i < NUM_OF_THREAD; ++i)
	{
		hThread[i] = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, NULL, CREATE_SUSPENDED, (unsigned int* )&dwThreadId[i]);

		if(hThread[i] == NULL)
		{
			printf_s("Thread Creation Fault! \n");
			return -1;
		}
	}

	for(DWORD i = 0; i < NUM_OF_THREAD; ++i)
	{
		ResumeThread(hThread[i]);
	}

	WaitForMultipleObjects(NUM_OF_THREAD, hThread, TRUE, INFINITE);

	printf_s("Total Count : %d \n", gTotalCount);

	for(DWORD i = 0; i < NUM_OF_THREAD; ++i)
	{
		CloseHandle(hThread[i]);
	}

	DeleteCriticalSection(&hCriticalSection);
	getchar();

	return 0;
}

