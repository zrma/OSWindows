// CPUStress.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>

#define		MAX_CORE_NUM		4

DWORD_PTR	gAffinityMaskSingleCore[] = { 0x0001, 0x0002, 0x0004, 0x0008, 0x0001 };
HANDLE		stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE); // 출력 핸들 추출 (현재 콘솔)

// 화면 지우기
void Clear(){
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;						// 화면 정보 구조체
	COORD origin = {0,0};										// 원점 기준
	DWORD length;
	GetConsoleScreenBufferInfo(stdOutputHandle, &screenInfo);	// 화면 정보를 얻는다.
	FillConsoleOutputCharacter(	stdOutputHandle, ' ', 
		screenInfo.dwSize.X * screenInfo.dwSize.Y, 
		origin, &length		);			// 화면 크기만큼 공백으로 채운다.
	SetConsoleCursorPosition(stdOutputHandle, origin);			// 커서를 이동시킨다.
}

unsigned int WINAPI	ThreadProc( LPVOID lpParam )
{
	if(0 == SetThreadAffinityMask(GetCurrentThread(), gAffinityMaskSingleCore[int(lpParam)]))
		printf_s("Affinity Fail %d \n", GetLastError());

	long temp = 0;

	while(true)
	{
		++temp;

		if(int(lpParam) == 4)
		{
			if(temp % 1500000 == 0)
			{
				Sleep(4);

				temp = 0;
			}
		}
	}
	// Busy Waiting...

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD	dwThreadId[MAX_CORE_NUM+1];
	HANDLE	hThread[MAX_CORE_NUM+1];

	Clear();
	printf_s("CPU 스트레스 테스트를 시작합니다. \n\n");

	for (int i = 0; i < MAX_CORE_NUM+1; ++i)
	{
		hThread[i] = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, (LPVOID) i, 
											CREATE_SUSPENDED, (unsigned int* ) &dwThreadId[i]);
	}
	
	printf_s("(1/6) CPU 전체 코어 점유율 100%c 테스트입니다. \n", '%');
	printf_s("테스트를 시작하기 위하여 [엔터]를 누르세요. \n");
	getchar();

	for (int i = 0; i < MAX_CORE_NUM; ++i)
	{
		ResumeThread(hThread[i]);

		
	}

	Clear();
	printf_s("CPU 스트레스 테스트 중입니다. \n\n");
	printf_s("(1/6) 과부하 구간에 진입하였습니다. \n");
	printf_s("전체 과부하 테스트를 마치려면 [엔터]를 누르세요. \n");
	getchar();

	Clear();
	printf_s("CPU 스트레스 테스트가 멈추어 있습니다. \n\n");
	printf_s("(2/6) CPU 단일 코어 점유율 100%c 테스트입니다. \n", '%');
	
	for (int i = 0; i < MAX_CORE_NUM; ++i)
	{
		SuspendThread(hThread[i]);
	}

	printf_s("테스트를 시작하기 위하여 [엔터]를 누르세요. \n");
	getchar();

	Clear();
	printf_s("CPU 스트레스 테스트가 재개 되었습니다. \n\n");

	for (int i = 0; i < MAX_CORE_NUM; ++i)
	{
	
		ResumeThread(hThread[i]);

		if(i != 0)
		{
			Clear();
			printf_s("CPU 스트레스 테스트 중입니다. \n\n");
		}
		printf_s("(%d/6) %d번 코어의 점유율이 100%c 입니다. \n", i+2, i, '%');

		if(i < MAX_CORE_NUM - 1)
		{
			printf_s("다음 코어를 진행하시려면 [엔터]를 누르세요. \n");
		}
		else
		{
			printf_s("단일 코어 스트레스 테스트를 마치려면 [엔터]를 누르세요. \n");
		}
		getchar();
		SuspendThread(hThread[i]);
	}
	
	Clear();
	printf_s("CPU 스트레스 테스트가 멈추어 있습니다. \n\n");
	printf_s("(6/6) 단일 코어 점유율 50%c 테스트입니다. \n", '%');
	printf_s("테스트를 시작하기 위하여 [엔터]를 누르세요. \n");
	getchar();

	ResumeThread(hThread[4]);

	Clear();
	printf_s("CPU 스트레스 테스트 중입니다. \n\n");
	printf_s("(6/6) 단일 코어 점유율 50%c 테스트 중입니다. \n", '%');
	printf_s("테스트를 종료하시려면 [엔터]를 누르세요. \n");
	getchar();

	SuspendThread(hThread[4]);

	for (int i = 0; i < MAX_CORE_NUM; ++i)
		CloseHandle(hThread[i]);

	Clear();
	printf_s("스트레스 테스트를 종료합니다. \n\n");
	printf_s("[엔터]를 누르면 프로그램이 종료됩니다. \n");

	getchar();

	return 0;
}

