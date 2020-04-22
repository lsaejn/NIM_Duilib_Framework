// ProcessGuard.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "windows.h"
#include <iostream>
#include <string>
#include <vector>

#include <tlhelp32.h>
#include <Psapi.h>
#pragma comment (lib,"Psapi.lib")

std::vector<DWORD> GetPidsOfChildProcess(DWORD parentPid)
{
	std::vector<DWORD> pidsOfChild;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return pidsOfChild;
	}
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	BOOL bProcess = Process32First(hProcessSnap, &pe);
	DWORD pidOfChild = 0;
	while (bProcess)
	{
		if (pe.th32ParentProcessID == parentPid)//判断如果父id与其pid相等，		
		{
			pidOfChild = pe.th32ProcessID;
			pidsOfChild.push_back(pidOfChild);
		}
		bProcess = Process32Next(hProcessSnap, &pe);
	}
	CloseHandle(hProcessSnap);
	return pidsOfChild;
}

int main(int argc, char** argv)
{
    if (argc != 2)
        return -1;
	
    DWORD pidOfPkpmV51 = std::stoi(argv[1]);
	auto pidsOfChild = GetPidsOfChildProcess(pidOfPkpmV51);
	auto pidOfThis=::GetCurrentProcessId();
	auto iter = std::find(pidsOfChild.cbegin(), pidsOfChild.cend(), pidOfThis);
	if (iter == pidsOfChild.cend())
		return -2;
	auto handle = OpenProcess(PROCESS_VM_OPERATION| SYNCHRONIZE, FALSE, pidOfPkpmV51);
	if (INVALID_HANDLE_VALUE == handle)
		return -1;
	auto ret=WaitForSingleObject(handle, INFINITE);
	if (ret == WAIT_OBJECT_0)
	{
		for (auto elem : pidsOfChild)
		{
			auto h=OpenProcess(PROCESS_TERMINATE, FALSE, pidOfPkpmV51);
			::TerminateProcess(h, -2);
		}			
	}
    std::cout << "bye!\n";
	return 0;
}

