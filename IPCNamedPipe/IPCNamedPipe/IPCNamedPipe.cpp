// IPCNamedPipe.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>
#include <string>

int _tmain(int argc, _TCHAR* argv[])
{
	std::string	pipeName = "\\\\.\\pipe\\testpipe";

	if(!WaitNamedPipeA(pipeName.c_str(), NMPWAIT_WAIT_FOREVER))
	{
		printf_s("Wait NamedPipe failed. Error=%d \n", GetLastError());
		return 0;
	}

	HANDLE handleWrite = CreateFileA(
		pipeName.c_str(), 
		GENERIC_WRITE,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if(handleWrite == INVALID_HANDLE_VALUE)
	{
		printf_s("Pipe open error %d \n", GetLastError());
		return 0;
	}

	char	buf[1024];
	DWORD	dwWritten;

	for(int i = 0; i < 5; i++)
	{
		sprintf_s(buf, "This is a test send. (Try %d)", i+1);

		if(!WriteFile(handleWrite, buf, (strlen(buf)+1)*2, &dwWritten, NULL))
		{
			printf_s("Write File failed. \n");
			break;
		}

		Sleep(1000);
	}

	CloseHandle(handleWrite);
	getchar();

	return 0;
}
