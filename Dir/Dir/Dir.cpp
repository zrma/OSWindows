// Dir.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>
#include <minwindef.h>

int _tmain(int argc, _TCHAR* argv[])
{
	char*	dirName = NULL;
	char	currentDirectory[MAX_PATH] = {0, };
	char	outString[255];
	GetCurrentDirectoryA(MAX_PATH, currentDirectory);

	if(argc > 1)
	{
		int size = wcslen(argv[1]);
		dirName = new char[++size];

		// 유니코드를 아스키로 변환
		WideCharToMultiByte(CP_ACP, 0, argv[1], size*2, dirName, size, NULL, false);
		strcpy_s(currentDirectory, dirName);

		if(dirName)
		{
			delete dirName;
			dirName = NULL;
		}
	}
	
	sprintf_s(currentDirectory, "%s.\\*", currentDirectory);
	
	WIN32_FIND_DATAA findFileData;
	HANDLE hFind = FindFirstFileA(currentDirectory, &findFileData);

	if(hFind == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFind);
		return -1;
	}

	sprintf_s(outString, "%-16s  /  %4s  /  %4s  /  %s \n", "Modified Time", "Size", "Type", "Name");
	fputs(outString, stdout);

	SYSTEMTIME	thisFileTime, localFileTime;
	do 
	{
		FileTimeToSystemTime(&findFileData.ftLastWriteTime, &thisFileTime);
		SystemTimeToTzSpecificLocalTime(NULL, &thisFileTime, &localFileTime);
		sprintf_s(outString, "%02d-%02d-%04d %02d:%02d   %8d %-8s   %s \n", 
			localFileTime.wMonth, localFileTime.wDay, localFileTime.wYear, localFileTime.wHour, localFileTime.wMinute,
			findFileData.nFileSizeLow, 
			(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? " <FOLD> " : " <FILE> ",
			findFileData.cFileName);

		fputs(outString, stdout);
	} while ( FindNextFileA(hFind, &findFileData) != 0);

	FindClose(hFind);

	return 0;
}

