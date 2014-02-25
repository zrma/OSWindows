// BusyWorker.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <stdlib.h>
#include <windows.h>

void Worker(wchar_t* str);

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc <= 1)
		return 0;

	int priority = _wtoi(argv[1]) % 3;

	switch (priority)
	{
	case 0:
		SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
		Worker(L"IDLE");
		break;
	case 1:
		SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
		Worker(L"HIGH");
		break;
	case 2:
		SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
		Worker(L"REAL");
		break;
	default:
		break;
	}

	getchar();
	return 0;
}

void Worker(wchar_t* str)
{
	for (int a = 0; a < 100; ++a)
	{
		for (int i = 0; i < 10000000; ++i)
		{
			; // busy waiting...
		}
		Sleep(0);
	}

	wprintf_s(L"%s END \n", str);
}
