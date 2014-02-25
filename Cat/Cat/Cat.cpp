// Cat.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>

bool ViewFile(char* fileName)
{
	HANDLE hReadFile = CreateFileA(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if(hReadFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hReadFile);
		printf_s("Read File Error - %s (%d)\n ", fileName, GetLastError());
		return false;
	}

	char	c = NULL;
	DWORD	numberOfByteRead = 0;
	BOOL	bResult = FALSE;

	while(true)
	{
		bResult = ReadFile(hReadFile, &c, sizeof(char), &numberOfByteRead, NULL);

		if(bResult == TRUE && numberOfByteRead == 0)
			break;
		// printf_s("%c", c);
		fputc(c, stdout);
	}

	CloseHandle(hReadFile);
	return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc < 2)
		return -1;

	char*	fileName;

	int size = wcslen(argv[1]);
	fileName = new char[++size];

	// 유니코드를 아스키로 변환
	WideCharToMultiByte(CP_ACP, 0, argv[1], size*2, fileName, size, NULL, false);

	ViewFile(fileName);

	if(fileName)
	{
		delete(fileName);
		fileName = NULL;
	}
	return 0;
}

