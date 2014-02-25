// MMFProcess1.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>

#define BUF_SIZE	256

char*	szName	= "Global\\MyFileMappingObject";
char*	szMsg	= "Message from first process.";

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hMapFile	= CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUF_SIZE, szName);

	if(hMapFile == NULL)
		return -1;

	char*	pBuf = (char* ) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);

	if(pBuf == NULL)
		return -1;

	memcpy(pBuf, szMsg, sizeof(char) * strlen(szMsg));

	getchar();
	
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);

	return 0;
}

