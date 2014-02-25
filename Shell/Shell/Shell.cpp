// Shell.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <TlHelp32.h>

std::string ERROR_COMMAND	= "\'%s\'��(��) ���� �� �� �ִ� ���α׷��� �ƴմϴ�. \n";
std::string ERROR_PARAMETER = "�Ű� ���� ������ �ùٸ��� �ʽ��ϴ�. \n";
std::string ERROR_CALL		= "����� ã�� �� �����ϴ�. \n";
std::string ERROR_MAKE		= "������ �����Ͽ����ϴ�. \n";
std::string ERROR_REMOVE	= "������ �����Ͽ����ϴ�. \n";
std::string ERROR_COPY		= "���翡 �����Ͽ����ϴ�. \n";
std::string ERROR_RENAME	= "�̸� ���濡 �����Ͽ����ϴ�. \n";
std::string ERROR_CALL_PS	= "���μ��� ��� ȣ�⿡ �����Ͽ����ϴ�. \n";
std::string ERROR_KILL_PROCESS = "\'%s\'��(��) �����ϴµ� �����Ͽ����ϴ�. \n";

bool	MainLoop();
bool	CreateProcessFunction(std::string commandString);

bool	ViewDirFunction(std::vector<std::string> commandArray);
bool	MakeDirFunction(std::string commandString);
bool	RemoveDirFunction(std::string commandString);
bool	ChangeDirFunction(std::string commandString);

bool	CopyFileFunction(std::string src, std::string dest);
bool	RenameFileFunction(std::string src, std::string dest);
bool	DeleteFileFunction(std::string commandString);
bool	ViewFileFunction(std::vector<std::string> commandArray);

bool	TaskListFunction();
bool	TaskKillFunction(std::string commandString);

void	Clear();

HANDLE	hStandardOut = GetStdHandle(STD_OUTPUT_HANDLE); // ��� �ڵ� ���� (���� �ܼ�)
HANDLE	hMutex;
DWORD	dwWaitResult;

int _tmain(int argc, _TCHAR* argv[])
{
	hMutex = CreateMutexA(NULL, FALSE, "ShellMutex");
	if(hMutex == NULL)
	{
		printf_s("Create Mutex Error : %d \n", GetLastError());
		return -1;
	}

	dwWaitResult = WaitForSingleObject(hMutex, 1);

	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
		break;
	case WAIT_TIMEOUT:
		printf_s("���� �̹� ���� ���Դϴ�. \n");
		return -1;
		break;
	case WAIT_ABANDONED:
		break;
	default:
		break;
	}

	bool	isContinue = true;

	while(isContinue)
		isContinue = MainLoop();

	printf_s("��ɾ� ó���� �����մϴ�. \n");
	//getchar();

	ReleaseMutex(hMutex);
	CloseHandle(hMutex);

	return 0;
}

bool MainLoop()
{
	std::string					inputString;
	std::string					tokken;
	std::vector<std::string>	commandArray;
	char						currentDirectory[MAX_PATH] = {0, };

	GetCurrentDirectoryA(MAX_PATH, currentDirectory);
	SetCurrentDirectoryA(currentDirectory);

	Sleep(100);
	std::cout << "MyShell " << currentDirectory << "> ";
	getline(std::cin, inputString);

	// ù ��° ���ڸ� �ҹ��ڷ� ��ȯ
	std::transform(inputString.begin(), inputString.end(), inputString.begin(), towlower);

	// ���ڿ���Ʈ������
	std::stringstream parseString(inputString);

	// �׽�Ʈ ���
	while (parseString >> tokken)
	{
		commandArray.push_back(tokken);
	}

	// �� ĭ�� �Է��ߴٸ� �н�
	if(!commandArray.size())
	{
		return true;
	}
	// ����
	else if(commandArray[0] == "exit" || commandArray[0] == "quit")
	{
		return false;
	}
	// ȭ�� �����
	else if(commandArray[0] == "clear")
	{
		Clear();
		return true;
	}
	// ���μ��� ��� Ȯ��
	else if(commandArray[0] == "ps" || commandArray[0] == "plist" || commandArray[0] == "pl")
	{
		if(!TaskListFunction())
		{
			printf_s(ERROR_CALL_PS.c_str());
			return true;
		}
	}
	// ���μ��� ����
	else if(commandArray[0] == "kill" || commandArray[0] == "pk")
	{
		if(commandArray.size() != 2)
		{
			printf_s(ERROR_PARAMETER.c_str());
			return true;
		}
		TaskKillFunction(commandArray[1]);
	}
	// ���丮 ����
	else if(commandArray[0] == "md" || commandArray[0] == "mkdir")
	{
		if(commandArray.size() != 2)
		{
			printf_s(ERROR_PARAMETER.c_str());
			return true;
		}
		else if(!MakeDirFunction(commandArray[1]))
		{
			printf_s(ERROR_MAKE.c_str());
			return true;
		}
	}
	// ���丮 ����
	else if(commandArray[0] == "rd" || commandArray[0] == "rmdir")
	{
		if(commandArray.size() != 2)
		{
			printf_s(ERROR_PARAMETER.c_str());
			return true;
		}
		else if(!RemoveDirFunction(commandArray[1]))
		{
			printf_s(ERROR_REMOVE.c_str());
			return true;
		}
	}
	// ���丮 �̵�
	else if(commandArray[0] == "cd")
	{
		if(commandArray.size() != 2)
		{
			printf_s(ERROR_PARAMETER.c_str());
			return true;
		}
		else if(!ChangeDirFunction(commandArray[1]))
		{
			printf_s(ERROR_CALL.c_str());
			return true;
		}
	}
	// ���丮 ���� ���
	else if(commandArray[0] == "dir")
	{
		if(!ViewDirFunction(commandArray))
		{
			printf_s(ERROR_CALL.c_str());
			return true;
		}
	}
	// ���� ����
	else if(commandArray[0] == "cp" || commandArray[0] == "copy")
	{
		if(commandArray.size() != 3)
		{
			printf_s(ERROR_PARAMETER.c_str());
			return true;
		}
		else if(!CopyFileFunction(commandArray[1], commandArray[2]))
		{
			printf_s(ERROR_COPY.c_str());
			return true;
		}
	}
	// ���� �̸� ����
	else if(commandArray[0] == "ren" || commandArray[0] == "rename")
	{
		if(commandArray.size() != 3)
		{
			printf_s(ERROR_PARAMETER.c_str());
			return true;
		}
		else if(!RenameFileFunction(commandArray[1], commandArray[2]))
		{
			printf_s(ERROR_RENAME.c_str());
			return true;
		}
	}
	// ���� ����
	else if(commandArray[0] == "del" || commandArray[0] == "rm")
	{
		if(commandArray.size() != 2)
		{
			printf_s(ERROR_PARAMETER.c_str());
			return true;
		}
		else if(!DeleteFileFunction(commandArray[1]))
		{
			printf_s(ERROR_REMOVE.c_str());
			return true;
		}
	}
	// ���� ���� ���
	else if(commandArray[0] == "cat")
	{
		if(commandArray.size() < 2 || (commandArray.size() > 2 && commandArray[2] != "|"))
		{
			printf_s(ERROR_PARAMETER.c_str());
			return true;
		}
		else if(!ViewFileFunction(commandArray))
		{
			printf_s(ERROR_CALL.c_str());
			return true;
		}
	}
	// ���μ��� ����
	else if(!CreateProcessFunction(inputString))
	{
		printf_s(ERROR_COMMAND.c_str(), commandArray[0].c_str());
	}
	printf_s("\n");

	return true;
}

// ���μ��� ����
bool CreateProcessFunction(std::string commandString)
{
	STARTUPINFOA		startupInfo;
	PROCESS_INFORMATION	processInformation;

	ZeroMemory(&startupInfo, sizeof(startupInfo));
	ZeroMemory(&processInformation, sizeof(processInformation));

	startupInfo.cb = sizeof(startupInfo);
	
	char	command[MAX_PATH] = {0, };

	if(commandString.length() > 250)
		return false;

	strcpy_s(command, commandString.c_str());

	char	currentDirectory[MAX_PATH] = {0, };

	GetCurrentDirectoryA(MAX_PATH, currentDirectory);
	SetCurrentDirectoryA(currentDirectory);
	
	// �ڽ� ���μ��� ����
	if(!CreateProcessA(NULL, command, NULL, NULL, false, NULL, NULL, NULL, &startupInfo, &processInformation))
	{
		return false;
	}

	// �ڵ� ���� <- �߿�!!
	CloseHandle(processInformation.hProcess);
	CloseHandle(processInformation.hThread);

	return true;
}

// DIR ��ɾ�
bool ViewDirFunction(std::vector<std::string> commandArray)
{
	char	commandStringWithOptions[MAX_PATH] = {0, };
	DWORD	result = 0;
	STARTUPINFOA si = {0, };
	PROCESS_INFORMATION pi;

	if(commandArray[0].size() > 250 || (commandArray.size() > 1 && commandArray[1].size() > 250) || 
		(commandArray.size() > 2 && commandArray[2].size() > 250) || (commandArray.size() > 3 && commandArray[3].size() > 250))
		return false;

	if(commandArray.size() > 2 && commandArray[commandArray.size()-2] == ">")
	{
		SECURITY_ATTRIBUTES fileSec = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

		HANDLE hFile = CreateFileA(commandArray[commandArray.size()-1].c_str(), GENERIC_WRITE, FILE_SHARE_READ,
									&fileSec, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		si.hStdOutput = hFile;
		si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
		si.dwFlags |= STARTF_USESTDHANDLES;

		strcpy_s(commandStringWithOptions, commandArray[0].c_str());
		for(int i = 1; i < static_cast<int>(commandArray.size() - 2); ++i)
		{
			sprintf_s(commandStringWithOptions, "%s %s", commandStringWithOptions, commandArray[i].c_str());
		}

		if(!CreateProcessA(NULL, commandStringWithOptions, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
			return false;

		result = WaitForSingleObject(pi.hProcess, INFINITE);
		
		if(hFile)
		{
			CloseHandle(hFile);
			hFile = NULL;
		}
		// �߿�! �� �ڵ��� ���� ������ ������ Ŀ�� ������Ʈ�� ī��Ʈ�� ���� ����.

		if(result == -1)
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			return false;
		}
	}
	else
	{
		strcpy_s(commandStringWithOptions, commandArray[0].c_str());
		for(int i = 1; i < static_cast<int>(commandArray.size()); ++i)
		{
			sprintf_s(commandStringWithOptions, "%s %s", commandStringWithOptions, commandArray[i]);
		}

		if(!CreateProcessA(NULL, commandStringWithOptions, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
			return false;

		result = WaitForSingleObject(pi.hProcess, INFINITE);
		
		if(result == -1)
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			return false;
		}
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return true;
}


// MKDIR
bool MakeDirFunction( std::string commandString )
{
	char	newDirectory[MAX_PATH] = {0, };
	char	currentDirectory[MAX_PATH] = {0, };

	GetCurrentDirectoryA(MAX_PATH, currentDirectory);
	SetCurrentDirectoryA(currentDirectory);

	strncpy_s(newDirectory, commandString.c_str(), commandString.size());
	
	if(CreateDirectoryA(newDirectory, NULL) == false)
		return false;

	return true;
}

// RMDIR
bool RemoveDirFunction( std::string commandString )
{
	char	newDirectory[MAX_PATH] = {0, };
	char	currentDirectory[MAX_PATH] = {0, };

	GetCurrentDirectoryA(MAX_PATH, currentDirectory);
	SetCurrentDirectoryA(currentDirectory);

	strncpy_s(newDirectory, commandString.c_str(), commandString.size());

	if(RemoveDirectoryA(newDirectory) == false)
		return false;

	return true;
}

// CHDIR
bool ChangeDirFunction( std::string commandString )
{
	char	newDirectory[MAX_PATH] = {0, };
	char	currentDirectory[MAX_PATH] = {0, };

	GetCurrentDirectoryA(MAX_PATH, currentDirectory);
	sprintf_s(newDirectory, "%s\\%s\\", currentDirectory, commandString.c_str());
	
	if(SetCurrentDirectoryA(newDirectory) == false)
		return false;
	return true;
}

// COPY
bool CopyFileFunction( std::string src, std::string dest )
{
	if(CopyFileA(src.c_str(), dest.c_str(), TRUE) == false)
		return false;

	return true;
}

// REN
bool RenameFileFunction( std::string src, std::string dest )
{
	if(MoveFileA(src.c_str(), dest.c_str()) == false)
		return false;

	return true;
}

// DEL
bool DeleteFileFunction( std::string commandString )
{
	if(DeleteFileA(commandString.c_str()) == false)
		return false;

	return true;
}

// CAT
bool ViewFileFunction( std::vector<std::string> commandArray )
{
	char	commandStringWithOptions[MAX_PATH] = {0, };

	if((commandArray[0].size() + commandArray[1].size() > 250) || (commandArray.size() > 3 && commandArray[3].size() > 250))
		return false;

	if(commandArray.size() > 3 && commandArray[2] == "|")
	{
		HANDLE hReadPipe, hWritePipe;
		SECURITY_ATTRIBUTES pipeSA = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

		// unnamed pipe ����
		CreatePipe(&hReadPipe, &hWritePipe, &pipeSA, 0);

		// process type�� ���� ����
		STARTUPINFOA siType = {0, };
		PROCESS_INFORMATION piType;
		siType.cb = sizeof(siType);

		siType.hStdInput = GetStdHandle(STD_INPUT_HANDLE);	// �Է��� ǥ���Է�����
		siType.hStdError = GetStdHandle(STD_ERROR_HANDLE);	// ������ ǥ�ؿ�����

		siType.hStdOutput = hWritePipe;						// ����� �������� (hWritePipe�� �������� ����� �ڵ�) 

		siType.dwFlags |= STARTF_USESTDHANDLES;				// ��Ʈ ���� |(or) ���� STARTF_USESTDHANDLES ���� �߰�
															// <- �ڽ� ���μ����� ǥ�� �ڵ��� ���� siType�� �̿��ؼ� �����ϰڴٴ� ��?

		// msdn ���� : DwFlags ����� STARTF_USESTDHANDLES�� ���� �Ǿ� ������ STARTUPINFO ��� �ܼ� ��� �ڽ� ���μ����� ǥ�� �ڵ��� ���� �մϴ�.

		sprintf_s(commandStringWithOptions, "%s %s", commandArray[0].c_str(), commandArray[1].c_str());

		if(!CreateProcessA(NULL, commandStringWithOptions, NULL, NULL, TRUE, 0, NULL, NULL, &siType, &piType))
		{
			return false;
		}

		CloseHandle(piType.hThread);
		CloseHandle(hWritePipe);
		// ���� �Ϸ�

		// process sort�� ���� ���� -> | ������ ó�� �޺κ� ��ɾ�
		// ������ ǥ�� ������� �������� �־��� �����͸� ���� ������ ���μ����� ǥ�� �Է����� �̾�޾Ƽ� ó����.

		STARTUPINFOA siSort = {0, };
		PROCESS_INFORMATION piSort;
		siSort.cb = sizeof(piSort);

		siSort.hStdInput = hReadPipe;						// �Է��� ���������� �ҷ��� (hReadPipe�� �������� �б�� �ڵ�)
		siSort.hStdError = GetStdHandle(STD_ERROR_HANDLE);	// ������ ǥ�� ����

		HANDLE hFile = NULL;

		if(commandArray.size() > 5 && commandArray[4] == ">")
		{
			SECURITY_ATTRIBUTES fileSec = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
			// ��� �����ؾ� ���� ����!

			hFile = CreateFileA(commandArray[5].c_str(), GENERIC_WRITE, FILE_SHARE_READ, &fileSec, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				printf_s("Create File Error - %s (%d) \n", commandArray[5].c_str(), GetLastError());
				return false;
			}

			siSort.hStdOutput = hFile;						// �����̷��� �Ϸ��� ����� ���� �ڵ��...
		}
		else 
			siSort.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);// ��µ� ǥ�� �������

		siSort.dwFlags |= STARTF_USESTDHANDLES;

		strcpy_s(commandStringWithOptions, commandArray[3].c_str());

		if(!CreateProcessA(NULL, commandStringWithOptions, NULL, NULL, TRUE, 0, NULL, NULL, &siSort, &piSort))
		{
			WaitForSingleObject(piType.hProcess, INFINITE);
			CloseHandle(piType.hProcess);

			return false;
		}

		CloseHandle(piSort.hThread);
		CloseHandle(hReadPipe);

		WaitForSingleObject(piType.hProcess, INFINITE);
		WaitForSingleObject(piSort.hProcess, INFINITE);

		CloseHandle(piType.hProcess);
		CloseHandle(piSort.hProcess);

		if(hFile)
		{
			CloseHandle(hFile);
			hFile = NULL;
		}

	}
	else
	{
		sprintf_s(commandStringWithOptions, "%s %s", commandArray[0].c_str(), commandArray[1].c_str());

		STARTUPINFOA si = {0, };
		PROCESS_INFORMATION pi;
		si.cb = sizeof(si);

		if(!CreateProcessA(NULL, commandStringWithOptions, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			return false;
		WaitForSingleObject(pi.hProcess, INFINITE);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	return true;
}

// ���μ��� ����Ʈ ���
bool TaskListFunction()
{
	HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if(hProcessSnapshot == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hProcessSnapshot);
		return false;
	}
	PROCESSENTRY32 processEntry32;
	processEntry32.dwSize = sizeof(PROCESSENTRY32);

	// ��� ó�� ��ġ��
	if(!Process32First(hProcessSnapshot, &processEntry32))
	{
		printf_s("Process 32 First ERROR. \n");
		CloseHandle(hProcessSnapshot);

		return false;
	}

	// ���������� ��� ���
	do 
	{
		_tprintf_s(L"%-10d  %s\n", processEntry32.th32ProcessID, processEntry32.szExeFile);
	} while (Process32Next(hProcessSnapshot, &processEntry32));

	CloseHandle(hProcessSnapshot);

	return true;
}

// ���μ��� ����
bool TaskKillFunction(std::string commandString)
{
	HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if(hProcessSnapshot == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hProcessSnapshot);
		return false;
	}
	PROCESSENTRY32 processEntry32;
	processEntry32.dwSize = sizeof(PROCESSENTRY32);

	if(!Process32First(hProcessSnapshot, &processEntry32))
	{
		printf_s("Process 32 First ERROR. \n");
		CloseHandle(hProcessSnapshot);

		return false;
	}

	HANDLE		hProcess;
	bool		isKill	= false;

	wchar_t*	processName;

	int			size = commandString.size();
	processName = new wchar_t[++size];

	// �ƽ�Ű(�̱� ����Ʈ ĳ����)�� �Է� ���� ���ڿ��� �����ڵ�(��Ƽ ����Ʈ ĳ����)�� ��ȯ	
	MultiByteToWideChar(CP_UTF8, 0, (char*)commandString.c_str(), size, processName, size*2);
	// _tprintf_s(_T("%s\n"), processName);

	do
	{
		// �̸��� �´��� Ȯ��
		if(_tcsicmp(processEntry32.szExeFile, processName) == 0)
		{
			// ������ �־��� ���μ����� PID�� �ڵ� ��ȯ
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processEntry32.th32ProcessID);

			if(hProcess != NULL)
			{
				// ���μ��� ���� ����
				TerminateProcess(hProcess, -1);
				isKill = true;
			}

			CloseHandle(hProcess);
			break;
		}

	} while(Process32Next(hProcessSnapshot, &processEntry32));

	CloseHandle(hProcessSnapshot);
	delete(processName);
	
	if(isKill == false)
	{
		printf_s(ERROR_KILL_PROCESS.c_str(), commandString.c_str());
	}
	return true;
}

// ȭ�� �����
void Clear(){
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;						// ȭ�� ���� ����ü
	COORD origin = {0,0};										// ���� ����
	DWORD length;

	GetConsoleScreenBufferInfo(hStandardOut, &screenInfo);		// ȭ�� ������ ��´�.
	FillConsoleOutputCharacter(	hStandardOut, ' ', 
								screenInfo.dwSize.X * screenInfo.dwSize.Y, 
								origin, &length		);			// ȭ�� ũ�⸸ŭ �������� ä���.
	SetConsoleCursorPosition(hStandardOut, origin);				// Ŀ���� �̵���Ų��.
}