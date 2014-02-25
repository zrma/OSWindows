// MemoryProtection.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <malloc.h>

int	data[] = { 1, 1, 2, 3, 4, 5, 6, 7 };

void TestFunc()
{
	printf_s("Hello World.....");
}

int _tmain(int argc, _TCHAR* argv[])
{
	int stackVar = 0xDEAD;

	void*	funcAddr = TestFunc;
	void*	dataAddr = (void* ) data;
	void*	headAddr = malloc(1024);

	printf_s("CODE  : %08x \n", funcAddr);
	printf_s("DATA  : %08x \n", dataAddr);
	printf_s("HEAP  : %08x \n", headAddr);
	printf_s("STACK : %08x \n", &stackVar);

	memcpy(funcAddr, &stackVar, sizeof(int));

	getchar();
	return 0;
}

