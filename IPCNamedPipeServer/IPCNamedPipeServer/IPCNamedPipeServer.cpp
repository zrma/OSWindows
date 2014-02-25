// IPCNamedPipeServer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//


#include "stdafx.h"
#include <string>
#include <windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	std::string pipeName = "\\\\.\\pipe\\testpipe";

	HANDLE handlePipe = CreateNamedPipeA(
		pipeName.c_str(), 
		PIPE_ACCESS_DUPLEX | WRITE_DAC,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		1024,		// Out	Buffer Size
		1024,		// In	Buffer Size
		NULL,
		NULL		);

	if(handlePipe == INVALID_HANDLE_VALUE)
		return 0;

	if(!ConnectNamedPipe(handlePipe, NULL))
		return 0;

	DWORD	dwBytesRead;
	char	buf[1024];

	while (true)
	{
		if (ReadFile(handlePipe, buf, sizeof(buf), &dwBytesRead, NULL))
		{
			buf[dwBytesRead] = '\0';
			printf_s("Read [%s]\n", buf);
		}
		else
			break;
	}

	DisconnectNamedPipe(handlePipe);
	CloseHandle(handlePipe);

	getchar();
	return 0;
}