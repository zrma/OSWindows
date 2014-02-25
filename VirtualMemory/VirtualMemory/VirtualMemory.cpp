// VirtualMemory.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <windows.h>

DWORD PAGE_SIZE	= 4096;						// 4KB
DWORD COMMIT_SIZE = 1024 * 100 * PAGE_SIZE;	// 1024 * 100 * 4KB = 400MB
DWORD RESERVE_SIZE = 3 * COMMIT_SIZE;		// 3	* 400MB		= 1.2GB

int _tmain(int argc, _TCHAR* argv[])
{
	SYSTEM_INFO	sysInfo;
	GetSystemInfo(&sysInfo);

	DWORD pageSize = sysInfo.dwPageSize;
	printf_s("Page Size : %d \n", pageSize);

	if(pageSize != PAGE_SIZE)
		return -1;

	LPVOID baseAddr = VirtualAlloc(NULL, RESERVE_SIZE, MEM_RESERVE, PAGE_NOACCESS);

	if(!baseAddr)
		return -1;

	char* startAllocAddr = (char* ) baseAddr;

	while(true)
	{
		printf_s("Press Any Number to Commit >> ");

		int in = 0;
		scanf_s("%d", &in);

		for(DWORD i = 0; i < COMMIT_SIZE; i += PAGE_SIZE)
		{
			LPVOID result = VirtualAlloc(startAllocAddr, PAGE_SIZE, MEM_COMMIT, PAGE_READWRITE);

			if(result)
			{
				char dummy[] = "dummy string~~~~~~";
				// Ŀ�Ը� �ǰ� �۾� ���տ� �ݿ� �Ǳ� ������
				// ������ ������ ���� ������ (COW) �Ǳ� ������

				strcpy_s(startAllocAddr, strlen(dummy)+1, dummy);
				startAllocAddr += PAGE_SIZE;
			}
			else
			{
				printf_s("Commit Failed!!! \n");
				getchar();
				break;
			}
		}
	}

	VirtualFree(baseAddr, 0, MEM_RESERVE);
	printf_s("End \n");
	system("pause");

	return 0;
}

