// Thread4.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>

#define	TOILET_NUM	3
#define PEOPLE_NUM	10

HANDLE	hSemaphore = NULL;

void TakeDump(int people)
{
	srand(GetCurrentThreadId());

	WaitForSingleObject(hSemaphore, INFINITE);

	printf_s("   >> IN %-2d \n", people);

	Sleep(rand() % 5000);

	printf_s("               %2d OUT >> \n", people);

	ReleaseSemaphore(hSemaphore, 1, NULL);
}

unsigned int WINAPI ThreadProc(LPVOID lpParam)
{
	TakeDump((int) lpParam);

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD	dwThreadId[PEOPLE_NUM]	= {0, };
	HANDLE	hThread[PEOPLE_NUM]	= {NULL, };

	hSemaphore = CreateSemaphore(NULL, TOILET_NUM, TOILET_NUM, NULL);
	if(!hSemaphore)
		return -1;

	for(int i = 0; i < PEOPLE_NUM; ++i)
	{
		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, (void* )(i+1), CREATE_SUSPENDED, (unsigned int* )&dwThreadId[i]);
		
		if(hThread[i] == NULL)
			return -1;
	}

	printf_s("======== DUMP START ======== \n");

	for(int i = 0; i < PEOPLE_NUM; ++i)
	{
		ResumeThread(hThread[i]);
	}

	WaitForMultipleObjects(PEOPLE_NUM, hThread, TRUE, INFINITE);

	printf_s("======== DUMP END   ======== \n");

	for(int i = 0; i < PEOPLE_NUM; ++i)
	{
		CloseHandle(hThread[i]);
	}

	CloseHandle(hSemaphore);

	getchar();

	return 0;
}

