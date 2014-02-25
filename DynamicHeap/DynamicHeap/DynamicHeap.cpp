// DynamicHeap.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>

#define PAGE_SIZE	4096

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hHeap = HeapCreate(HEAP_NO_SERIALIZE, PAGE_SIZE*10, PAGE_SIZE*100);

	int* pArr = (int* ) HeapAlloc(hHeap, 0, sizeof(int) * 30);
	
	for(int i = 0 ; i < 10 ; ++i)
	{
		if(i % 5 == 0)
			printf_s("\n");
		pArr[i] = (i + 1) * 100;
	}

	for(int i = 0 ; i < 30 ; ++i)
	{
		if(i % 5 == 0)
		printf_s("\n");
		printf_s("%d ", pArr[i]);
	}
	HeapFree(hHeap, 0, pArr);
	HeapDestroy(hHeap);
	
	getchar();

	return 0;
}

