// SaveLoad.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>
#include <assert.h>

#define	MAX_PLAYER	10

// #pragma	pack(1)			// Memory Alignment 바이트 수
// #pragma	pack(4)

struct PlayerData
{
	int		m_PlayerIndex;	// 4 Byte		-> 4 Byte
	bool	m_IsMale;		// 1 Byte		-> 4 Byte
	short	m_HP;			// 2 Byte		-> 4 Byte
	__int64	m_PosX;			// 8 Byte		-> 4 Byte + 4 Byte
	__int64	m_PosY;			// 8 Byte		-> 4 Byte + 4 Byte
	short	m_Stage;		// 2 Byte		-> 4 Byte
};
							// 총합 25 Byte
							// 실제 저장 된 용량 = 32 Byte

							// CPU가 저장 하기 쉽게 4 Byte [4byte * 8bit = 32bit] 단위로 저장

							// Memory Alignment 에 따름.

PlayerData	g_PlayerSaveData[MAX_PLAYER];

bool SavePlayerData()
{
	for(int i = 0; i < MAX_PLAYER; ++i)
	{
		g_PlayerSaveData[i].m_PlayerIndex	= i;
		g_PlayerSaveData[i].m_IsMale		= (i % 2 == 0) ? true : false;
		g_PlayerSaveData[i].m_HP			= (i + 1) * 100;
		g_PlayerSaveData[i].m_PosX			= rand() * 1000;
		g_PlayerSaveData[i].m_PosY			= rand() * 1000;
		g_PlayerSaveData[i].m_Stage			= rand() % 500;
	}

	HANDLE hSaveFile = CreateFileA("Players.dat", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if(hSaveFile == INVALID_HANDLE_VALUE)
	{
		printf_s("Create File Error - players.dat : %d \n", GetLastError());

		return false;
	}

	DWORD numOfByteWritten	= 0;
	WriteFile(hSaveFile, g_PlayerSaveData, sizeof(g_PlayerSaveData), &numOfByteWritten, NULL);
	CloseHandle(hSaveFile);

	printf_s("Total  Player Data Size : %d \n", sizeof(g_PlayerSaveData));
	printf_s("Total Written Data Size : %d \n\n", numOfByteWritten);

	printf_s("[ID:%d] [M:%d] [HP:%d] [X:%d] [Y:%d] [S:%d] \n",
		-(int)(&g_PlayerSaveData[0].m_PlayerIndex)+(int)(&g_PlayerSaveData[0].m_IsMale), 
		-(int)(&g_PlayerSaveData[0].m_IsMale)+(int)(&g_PlayerSaveData[0].m_HP), 
		-(int)(&g_PlayerSaveData[0].m_HP)+(int)(&g_PlayerSaveData[0].m_PosX), 
		-(int)(&g_PlayerSaveData[0].m_PosX)+(int)(&g_PlayerSaveData[0].m_PosY), 
		-(int)(&g_PlayerSaveData[0].m_PosY)+(int)(&g_PlayerSaveData[0].m_Stage), 
		-(int)(&g_PlayerSaveData[0].m_Stage)+(int)(&g_PlayerSaveData[1].m_PlayerIndex));

	return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(false == SavePlayerData())
		return 0;

	HANDLE hLoadFile = CreateFileA("Players.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(hLoadFile == INVALID_HANDLE_VALUE)
	{
		printf_s("Read File Error - players.dat : %d \n", GetLastError());

		return 0;
	}

	int playerIndex = -1;
	PlayerData	loadData;

	while(true)
	{
		printf_s("Input Player Index : ");
		scanf_s("%d", &playerIndex);

		if(playerIndex < 0 || playerIndex >= MAX_PLAYER)
			break;
		
		// 포인터 이동 - 원하는 레코드가 있는 곳으로
		DWORD pos = SetFilePointer(hLoadFile, playerIndex * sizeof(PlayerData), NULL, FILE_BEGIN);

		if(pos == INVALID_SET_FILE_POINTER)
			break;

		// 레코드 읽어옴
		DWORD numberOfByteRead = 0;
		ReadFile(hLoadFile, &loadData, sizeof(PlayerData), &numberOfByteRead, NULL);

		// 제대로 읽어 왔는지 검증
		assert( g_PlayerSaveData[playerIndex].m_PlayerIndex == loadData.m_PlayerIndex);
		assert( g_PlayerSaveData[playerIndex].m_Stage == loadData.m_Stage);

		printf_s("Player Index:%d, Male:%d, HP:%d, X:%I64d, Y:%I64d, Stage:%d \n",
				loadData.m_PlayerIndex, loadData.m_IsMale, loadData.m_PosX, loadData.m_PosY, loadData.m_Stage);
	}

	CloseHandle(hLoadFile);

	getchar();
	return 0;
}

