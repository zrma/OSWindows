// MailSender.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>

#define		SLOT_NAME	"\\\\.\\mailslot\\mailbox"

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE	handleMailSlot;
	char	message[80];
	DWORD	bytesWritten;

	handleMailSlot = CreateFileA(
						SLOT_NAME,
						GENERIC_WRITE,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL
								);

	if(handleMailSlot == INVALID_HANDLE_VALUE)
	{
		fputs("Unable to create mailslot! \n", stdout);

		return 1;
	}

	while(true)
	{
		fputs("MESSAGE> ", stdout);
		fgets(message, sizeof(message) / sizeof(char), stdin);

		if(!WriteFile(handleMailSlot, message, strlen(message) * sizeof(char), &bytesWritten, NULL))
		{
			fputs("Unable to Write! \n", stdout);
			CloseHandle(handleMailSlot);

			return 1;
		}

		if(strncmp(message, "exit", 4) == 0)
		{
			fputs("Good Bye! \n", stdout);
			break;
		}
	}

	CloseHandle(handleMailSlot);
	return 0;
}

