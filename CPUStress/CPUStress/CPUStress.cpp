// CPUStress.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>

#define		MAX_CORE_NUM		4

DWORD_PTR	gAffinityMaskSingleCore[] = { 0x0001, 0x0002, 0x0004, 0x0008, 0x0001 };
HANDLE		stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE); // ��� �ڵ� ���� (���� �ܼ�)

// ȭ�� �����
void Clear(){
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;						// ȭ�� ���� ����ü
	COORD origin = {0,0};										// ���� ����
	DWORD length;
	GetConsoleScreenBufferInfo(stdOutputHandle, &screenInfo);	// ȭ�� ������ ��´�.
	FillConsoleOutputCharacter(	stdOutputHandle, ' ', 
		screenInfo.dwSize.X * screenInfo.dwSize.Y, 
		origin, &length		);			// ȭ�� ũ�⸸ŭ �������� ä���.
	SetConsoleCursorPosition(stdOutputHandle, origin);			// Ŀ���� �̵���Ų��.
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
	printf_s("CPU ��Ʈ���� �׽�Ʈ�� �����մϴ�. \n\n");

	for (int i = 0; i < MAX_CORE_NUM+1; ++i)
	{
		hThread[i] = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, (LPVOID) i, 
											CREATE_SUSPENDED, (unsigned int* ) &dwThreadId[i]);
	}
	
	printf_s("(1/6) CPU ��ü �ھ� ������ 100%c �׽�Ʈ�Դϴ�. \n", '%');
	printf_s("�׽�Ʈ�� �����ϱ� ���Ͽ� [����]�� ��������. \n");
	getchar();

	for (int i = 0; i < MAX_CORE_NUM; ++i)
	{
		ResumeThread(hThread[i]);

		
	}

	Clear();
	printf_s("CPU ��Ʈ���� �׽�Ʈ ���Դϴ�. \n\n");
	printf_s("(1/6) ������ ������ �����Ͽ����ϴ�. \n");
	printf_s("��ü ������ �׽�Ʈ�� ��ġ���� [����]�� ��������. \n");
	getchar();

	Clear();
	printf_s("CPU ��Ʈ���� �׽�Ʈ�� ���߾� �ֽ��ϴ�. \n\n");
	printf_s("(2/6) CPU ���� �ھ� ������ 100%c �׽�Ʈ�Դϴ�. \n", '%');
	
	for (int i = 0; i < MAX_CORE_NUM; ++i)
	{
		SuspendThread(hThread[i]);
	}

	printf_s("�׽�Ʈ�� �����ϱ� ���Ͽ� [����]�� ��������. \n");
	getchar();

	Clear();
	printf_s("CPU ��Ʈ���� �׽�Ʈ�� �簳 �Ǿ����ϴ�. \n\n");

	for (int i = 0; i < MAX_CORE_NUM; ++i)
	{
	
		ResumeThread(hThread[i]);

		if(i != 0)
		{
			Clear();
			printf_s("CPU ��Ʈ���� �׽�Ʈ ���Դϴ�. \n\n");
		}
		printf_s("(%d/6) %d�� �ھ��� �������� 100%c �Դϴ�. \n", i+2, i, '%');

		if(i < MAX_CORE_NUM - 1)
		{
			printf_s("���� �ھ �����Ͻ÷��� [����]�� ��������. \n");
		}
		else
		{
			printf_s("���� �ھ� ��Ʈ���� �׽�Ʈ�� ��ġ���� [����]�� ��������. \n");
		}
		getchar();
		SuspendThread(hThread[i]);
	}
	
	Clear();
	printf_s("CPU ��Ʈ���� �׽�Ʈ�� ���߾� �ֽ��ϴ�. \n\n");
	printf_s("(6/6) ���� �ھ� ������ 50%c �׽�Ʈ�Դϴ�. \n", '%');
	printf_s("�׽�Ʈ�� �����ϱ� ���Ͽ� [����]�� ��������. \n");
	getchar();

	ResumeThread(hThread[4]);

	Clear();
	printf_s("CPU ��Ʈ���� �׽�Ʈ ���Դϴ�. \n\n");
	printf_s("(6/6) ���� �ھ� ������ 50%c �׽�Ʈ ���Դϴ�. \n", '%');
	printf_s("�׽�Ʈ�� �����Ͻ÷��� [����]�� ��������. \n");
	getchar();

	SuspendThread(hThread[4]);

	for (int i = 0; i < MAX_CORE_NUM; ++i)
		CloseHandle(hThread[i]);

	Clear();
	printf_s("��Ʈ���� �׽�Ʈ�� �����մϴ�. \n\n");
	printf_s("[����]�� ������ ���α׷��� ����˴ϴ�. \n");

	getchar();

	return 0;
}

