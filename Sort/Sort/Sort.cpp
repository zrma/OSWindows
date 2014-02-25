// Sort.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <windows.h>
#include <string.h>

#define MAX_STRING_NUM		100
#define MAX_STRING_LEN		256

int compare(const void* op1, const void* op2)
{
	return strcmp((char* ) op1, (char* ) op2);
}

void SortString()
{
	char stringArr[MAX_STRING_NUM][MAX_STRING_LEN] = {0, };

	int nStrNumber;
	for(nStrNumber = 0; nStrNumber < MAX_STRING_NUM; ++nStrNumber)
	{
		char* isEOF = fgets(stringArr[nStrNumber], MAX_STRING_LEN, stdin);

		if(isEOF == NULL)
			break;
	}

	qsort(stringArr, nStrNumber, MAX_STRING_LEN, compare);

	for(int i = 0; i < nStrNumber; ++i)
		fputs(stringArr[i], stdout);
}

int _tmain(int argc, _TCHAR* argv[])
{
	bool	redirectFlag = false;
	char*	fileName = NULL;

	SortString();

	return 0;
}

