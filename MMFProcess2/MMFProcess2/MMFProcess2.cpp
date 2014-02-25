// MMFProcess2.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>

#define BUF_SIZE	256

char*	szName	= "Global\\MyFileMappingObject";

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, szName);

	if(hMapFile == NULL)
		return -1;

	char* pBuf = (char* ) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);

	if(pBuf == NULL)
		return -1;

	printf_s("%s \n", pBuf);

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);

	getchar();
	return 0;
}

