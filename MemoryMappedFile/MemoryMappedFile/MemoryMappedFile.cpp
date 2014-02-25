// MemoryMappedFile.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>

DWORD MAX_MAP_SIZE	= 1024 * 10; // 메모리 맵핑 크기

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hFile = CreateFileA("data.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return -1;

	HANDLE hMapFile = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, 0, MAX_MAP_SIZE, NULL);
	if(hMapFile == NULL)
		return -2;

	char* pWrite = (char *) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if(pWrite == NULL)
		return -3;

	char* tempString = "012345678901234567890123456789~~~~~ Test!!!";
	strcpy_s(pWrite, strlen(tempString) + 1, tempString);

	char baseChar = 'A';
	for(int i = 0; i < 26; ++i)
	{
		pWrite[i] = baseChar++;
	}

	FlushViewOfFile(pWrite, NULL);

	CloseHandle(hMapFile);
	CloseHandle(hFile);
	system("pause");

	return 0;
}

