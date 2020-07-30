#include "pch.h"
#include "IteratorHelper.h"
#include <tlhelp32.h>
#include <Psapi.h>


class Win32HandleGuard
{
public:
	Win32HandleGuard(HANDLE h)
		:h_(h)
	{
	}

	~Win32HandleGuard()
	{
		if (INVALID_HANDLE_VALUE != h_)
			::CloseHandle(h_);
	}
private:
	HANDLE h_;
};



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
		Win32HandleGuard guard(hProcessSnap);
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(pe);
		BOOL bProcess = Process32First(hProcessSnap, &pe);
		while (bProcess)
		{
			if (GetHandleFromPid(pe.th32ProcessID) == handle)
				return pe.th32ProcessID;
			bProcess = Process32Next(hProcessSnap, &pe);
		}
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