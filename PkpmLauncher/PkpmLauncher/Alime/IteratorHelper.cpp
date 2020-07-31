#include "pch.h"
#include "IteratorHelper.h"
#include <tlhelp32.h>
#include <Psapi.h>

namespace ProcessHelper
{
	Pid GetCurrentPid()
	{
		return ::GetCurrentProcessId();
	}

	HANDLE GetCurrentHandle()
	{
		return ::GetCurrentProcess();
	}

	HANDLE GetHandleFromPid(Pid pid)
	{
		HANDLE handle = INVALID_HANDLE_VALUE;
		handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		if (INVALID_HANDLE_VALUE == handle)
			handle = OpenProcess(PROCESS_VM_READ, FALSE, pid);
		return handle;
	}

	Pid GetPidFromHandle(HANDLE handle)
	{
		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			return NULL;
		}
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(pe);
		BOOL bProcess = Process32First(hProcessSnap, &pe);
		while (bProcess)
		{
			if (GetHandleFromPid(pe.th32ProcessID) == handle)
			{
				::CloseHandle(hProcessSnap);
				return pe.th32ProcessID;
			}
				
			bProcess = Process32Next(hProcessSnap, &pe);
		}
		::CloseHandle(hProcessSnap);
		return NULL;
	}

	void KillProcess(HANDLE handle)
	{
		::TerminateProcess(handle, 1);
	}

	void KillProcess(Pid pid)
	{
		KillProcess(GetHandleFromPid(pid));
	}

}