// Shell.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
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

std::string ERROR_COMMAND	= "\'%s\'은(는) 실행 할 수 있는 프로그램이 아닙니다. \n";
std::string ERROR_PARAMETER = "매개 변수 형식이 올바르지 않습니다. \n";
std::string ERROR_CALL		= "대상을 찾을 수 없습니다. \n";
std::string ERROR_MAKE		= "생성에 실패하였습니다. \n";
std::string ERROR_REMOVE	= "삭제에 실패하였습니다. \n";
std::string ERROR_COPY		= "복사에 실패하였습니다. \n";
std::string ERROR_RENAME	= "이름 변경에 실패하였습니다. \n";
std::string ERROR_CALL_PS	= "프로세스 목록 호출에 실패하였습니다. \n";
std::string ERROR_KILL_PROCESS = "\'%s\'을(를) 종료하는데 실패하였습니다. \n";

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

HANDLE	hStandardOut = GetStdHandle(STD_OUTPUT_HANDLE); // 출력 핸들 추출 (현재 콘솔)
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
		printf_s("쉘이 이미 실행 중입니다. \n");
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

	printf_s("명령어 처리를 종료합니다. \n");
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

	// 첫 번째 인자를 소문자로 변환
	std::transform(inputString.begin(), inputString.end(), inputString.begin(), towlower);

	// 문자열스트림으로
	std::stringstream parseString(inputString);

	// 테스트 출력
	while (parseString >> tokken)
	{
		commandArray.push_back(tokken);
	}

	// 빈 칸을 입력했다면 패스
	if(!commandArray.size())
	{
		return true;
	}
	// 종료
	else if(commandArray[0] == "exit" || commandArray[0] == "quit")
	{
		return false;
	}
	// 화면 지우기
	else if(commandArray[0] == "clear")
	{
		Clear();
		return true;
	}
	// 프로세스 목록 확인
	else if(commandArray[0] == "ps" || commandArray[0] == "plist" || commandArray[0] == "pl")
	{
		if(!TaskListFunction())
		{
			printf_s(ERROR_CALL_PS.c_str());
			return true;
		}
	}
	// 프로세스 종료
	else if(commandArray[0] == "kill" || commandArray[0] == "pk")
	{
		if(commandArray.size() != 2)
		{
			printf_s(ERROR_PARAMETER.c_str());
			return true;
		}
		TaskKillFunction(commandArray[1]);
	}
	// 디렉토리 생성
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
	// 디렉토리 삭제
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
	// 디렉토리 이동
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
	// 디렉토리 내부 출력
	else if(commandArray[0] == "dir")
	{
		if(!ViewDirFunction(commandArray))
		{
			printf_s(ERROR_CALL.c_str());
			return true;
		}
	}
	// 파일 복사
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
	// 파일 이름 변경
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
	// 파일 삭제
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
	// 파일 내용 출력
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
	// 프로세스 생성
	else if(!CreateProcessFunction(inputString))
	{
		printf_s(ERROR_COMMAND.c_str(), commandArray[0].c_str());
	}
	printf_s("\n");

	return true;
}

// 프로세스 생성
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
	
	// 자식 프로세스 생성
	if(!CreateProcessA(NULL, command, NULL, NULL, false, NULL, NULL, NULL, &startupInfo, &processInformation))
	{
		return false;
	}

	// 핸들 삭제 <- 중요!!
	CloseHandle(processInformation.hProcess);
	CloseHandle(processInformation.hThread);

	return true;
}

// DIR 명령어
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
		// 중요! 이 핸들을 닫지 않으면 파일의 커널 오브젝트에 카운트가 남아 있음.

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

		// unnamed pipe 생성
		CreatePipe(&hReadPipe, &hWritePipe, &pipeSA, 0);

		// process type을 위한 선언
		STARTUPINFOA siType = {0, };
		PROCESS_INFORMATION piType;
		siType.cb = sizeof(siType);

		siType.hStdInput = GetStdHandle(STD_INPUT_HANDLE);	// 입력은 표준입력으로
		siType.hStdError = GetStdHandle(STD_ERROR_HANDLE);	// 에러도 표준에러로

		siType.hStdOutput = hWritePipe;						// 출력을 파이프로 (hWritePipe는 파이프의 쓰기용 핸들) 

		siType.dwFlags |= STARTF_USESTDHANDLES;				// 비트 연산 |(or) 으로 STARTF_USESTDHANDLES 설정 추가
															// <- 자식 프로세스의 표준 핸들을 위의 siType을 이용해서 지정하겠다는 듯?

		// msdn 참고 : DwFlags 멤버가 STARTF_USESTDHANDLES로 설정 되어 있으면 STARTUPINFO 멤버 콘솔 기반 자식 프로세스의 표준 핸들을 지정 합니다.

		sprintf_s(commandStringWithOptions, "%s %s", commandArray[0].c_str(), commandArray[1].c_str());

		if(!CreateProcessA(NULL, commandStringWithOptions, NULL, NULL, TRUE, 0, NULL, NULL, &siType, &piType))
		{
			return false;
		}

		CloseHandle(piType.hThread);
		CloseHandle(hWritePipe);
		// 쓰기 완료

		// process sort를 위한 선언 -> | 파이프 처리 뒷부분 명령어
		// 위에서 표준 출력으로 파이프에 넣어준 데이터를 새로 생성한 프로세스가 표준 입력으로 이어받아서 처리함.

		STARTUPINFOA siSort = {0, };
		PROCESS_INFORMATION piSort;
		siSort.cb = sizeof(piSort);

		siSort.hStdInput = hReadPipe;						// 입력을 파이프에서 불러옴 (hReadPipe는 파이프의 읽기용 핸들)
		siSort.hStdError = GetStdHandle(STD_ERROR_HANDLE);	// 에러는 표준 에러

		HANDLE hFile = NULL;

		if(commandArray.size() > 5 && commandArray[4] == ">")
		{
			SECURITY_ATTRIBUTES fileSec = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
			// 상속 가능해야 지정 가능!

			hFile = CreateFileA(commandArray[5].c_str(), GENERIC_WRITE, FILE_SHARE_READ, &fileSec, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile == INVALID_HANDLE_VALUE)
			{
				printf_s("Create File Error - %s (%d) \n", commandArray[5].c_str(), GetLastError());
				return false;
			}

			siSort.hStdOutput = hFile;						// 리다이렉션 하려면 출력을 파일 핸들로...
		}
		else 
			siSort.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);// 출력도 표준 출력으로

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

// 프로세스 리스트 출력
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

	// 목록 처음 위치로
	if(!Process32First(hProcessSnapshot, &processEntry32))
	{
		printf_s("Process 32 First ERROR. \n");
		CloseHandle(hProcessSnapshot);

		return false;
	}

	// 순차적으로 목록 출력
	do 
	{
		_tprintf_s(L"%-10d  %s\n", processEntry32.th32ProcessID, processEntry32.szExeFile);
	} while (Process32Next(hProcessSnapshot, &processEntry32));

	CloseHandle(hProcessSnapshot);

	return true;
}

// 프로세스 종료
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

	// 아스키(싱글 바이트 캐릭터)로 입력 받은 문자열을 유니코드(멀티 바이트 캐릭터)로 변환	
	MultiByteToWideChar(CP_UTF8, 0, (char*)commandString.c_str(), size, processName, size*2);
	// _tprintf_s(_T("%s\n"), processName);

	do
	{
		// 이름이 맞는지 확인
		if(_tcsicmp(processEntry32.szExeFile, processName) == 0)
		{
			// 위에서 주어진 프로세스의 PID로 핸들 반환
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, processEntry32.th32ProcessID);

			if(hProcess != NULL)
			{
				// 프로세스 강제 종료
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

// 화면 지우기
void Clear(){
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;						// 화면 정보 구조체
	COORD origin = {0,0};										// 원점 기준
	DWORD length;

	GetConsoleScreenBufferInfo(hStandardOut, &screenInfo);		// 화면 정보를 얻는다.
	FillConsoleOutputCharacter(	hStandardOut, ' ', 
								screenInfo.dwSize.X * screenInfo.dwSize.Y, 
								origin, &length		);			// 화면 크기만큼 공백으로 채운다.
	SetConsoleCursorPosition(hStandardOut, origin);				// 커서를 이동시킨다.
}