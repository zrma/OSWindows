// Deadlock.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>

#define	PLAYER_COUNT	4
#define RUNNING_TIME	100

struct	SPlayer
{
	int					m_PlayerID;
	int					m_HP;

	CRITICAL_SECTION	m_Lock;
};

SPlayer	PlayerData[PLAYER_COUNT];

void	DoBloodSucking(SPlayer* from, SPlayer* to)
{
	printf_s("BEGIN : from <%d> to <%d> \n", from->m_PlayerID, to->m_PlayerID);

	EnterCriticalSection(((&from->m_Lock < &to->m_Lock) ? &from->m_Lock : &to->m_Lock));
	EnterCriticalSection(((&from->m_Lock < &to->m_Lock) ? &to->m_Lock : &from->m_Lock));
	// EnterCriticalSection(&from->m_Lock);
	// EnterCriticalSection(&to->m_Lock);

	to->m_HP	-= 3000;
	from->m_HP	+= 3000;

	Sleep(100);

	LeaveCriticalSection(&to->m_Lock);
	LeaveCriticalSection(&from->m_Lock);
	
	printf_s("                                  END   : from <%d> to <%d> \n", from->m_PlayerID, to->m_PlayerID);
}

unsigned int WINAPI	ThreadProc( LPVOID lpParam )
{
	srand(GetCurrentThreadId());

	int from = (int) lpParam;

	// from이 랜덤하게 대상 to를 정하여 흡혈
	for(int i = 0; i < RUNNING_TIME; ++i)
	{
		int to = 0;

		while(true)
		{
			to = rand() % PLAYER_COUNT;

			if(from != to)
				break;
		}
		
		DoBloodSucking(&PlayerData[from], &PlayerData[to]);
	}

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD	dwThreadId[PLAYER_COUNT];
	HANDLE	hThread[PLAYER_COUNT];

	// HP 채우고 초기화
	for (int i = 0; i < PLAYER_COUNT; ++i)
	{
		PlayerData[i].m_PlayerID = i + 101;
		PlayerData[i].m_HP = 100000;

		InitializeCriticalSection(&(PlayerData[i].m_Lock));
	}

	for (DWORD i = 0; i < PLAYER_COUNT; ++i)
	{
		hThread[i] = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, (LPVOID) i, 
											CREATE_SUSPENDED, (unsigned int* ) &dwThreadId[i]);
	}

	printf_s("==================== BEGIN ==================== \n");

	for (DWORD i = 0; i < PLAYER_COUNT; ++i)
		ResumeThread(hThread[i]);

	WaitForMultipleObjects(PLAYER_COUNT, hThread, TRUE, INFINITE);

	printf_s("====================  END  ==================== \n");

	int totalHP = 0;
	// 최종 HP 현황
	for (int i = 0; i < PLAYER_COUNT; ++i)
	{
		totalHP += PlayerData[i].m_HP;

		printf_s("  Player <%d> HP : %d \n", PlayerData[i].m_PlayerID, PlayerData[i].m_HP);
		DeleteCriticalSection(&(PlayerData[i].m_Lock));
	}

	printf_s(" Total HP : %d \n", totalHP);

	for (DWORD i = 0; i < PLAYER_COUNT; ++i)
		CloseHandle(hThread[i]);

	getchar();

	return 0;
}

