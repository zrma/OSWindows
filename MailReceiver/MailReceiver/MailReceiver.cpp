// MailReceiver.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>

#define		SLOT_NAME	"\\\\.\\mailslot\\mailbox"

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE	handleMailSlot;
	char	messageBox[80];
	DWORD	bytesRead;

	handleMailSlot = CreateMailslotA(
						SLOT_NAME,
						0,
						MAILSLOT_WAIT_FOREVER,
						NULL
									);

	if(handleMailSlot == INVALID_HANDLE_VALUE)
	{
		fputs("Unable to create mailslot! \n", stdout);

		return 1;
	}

	fputs("********** MESSAGE ********** \n", stdout);

	while(true)
	{
		if(!ReadFile(handleMailSlot, messageBox, sizeof(char) * 80, &bytesRead, NULL))
		{
			fputs("Unable to Read! \n", stdout);
			CloseHandle(handleMailSlot);

			return 1;
		}

		if(strncmp(messageBox, "exit", 4) == 0)
		{
			fputs("Good Bye! \n", stdout);
			break;
		}

		messageBox[bytesRead/sizeof(char)] = 0;	// NULL 문자 삽입
		fputs(messageBox, stdout);
	}

	CloseHandle(handleMailSlot);

	return 0;
}

