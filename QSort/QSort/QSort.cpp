// QSort.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <string.h>
#include <windows.h>

int compare(const void* op1, const void* op2)
{
	printf_s("%s and %s compare \n", (char* ) op1, (char* ) op2);
	return strcmp((char* ) op1, (char* ) op2);
}

int _tmain(int argc, _TCHAR* argv[])
{
	char stringArr[8][8] = { 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A',
							  'B', 'A', 'B', 'A', 'B', 'A', 'B', 'A',
							  'C', 'A', 'C', 'A', 'C', 'A', 'C', 'A',
							  'D', 'A', 'D', 'A', 'D', 'A', 'D', 'A',
							  'E', 'A', 'E', 'A', 'E', 'A', 'E', 'A',
							  'F', 'A', 'F', 'A', 'F', 'A', 'F', 'A',
							  'G', 'A', 'G', 'A', 'G', 'A', 'G', 'A',
							  'H', 'A', 'H', 'A', 'H', 'A', 'H', 'A',
	};

	qsort(stringArr, 8, 8, compare);

	getchar();
	return 0;
}

