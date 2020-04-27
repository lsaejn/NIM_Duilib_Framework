#include "pch.h"
#include "ProcessInfo.h"

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

namespace 
{
	typedef struct
	{
		HWND hwndWindow; // 窗口句柄
		DWORD dwProcessID; // 进程ID
	}EnumWindowsArg;

	BOOL CALLBACK EnumWindowsHwndFunc(HWND hwnd, LPARAM lParam)
	{
		EnumWindowsArg* pArg = (EnumWindowsArg*)lParam;
		DWORD dwProcessID = 0;
		::GetWindowThreadProcessId(hwnd, &dwProcessID);
		if (dwProcessID == pArg->dwProcessID)
		{
			pArg->hwndWindow = hwnd;
			return FALSE;
		}
		return TRUE;
	}
}

namespace ProcessInspector
{
	DWORD GetCurrentPid()
	{
		return ::GetCurrentProcessId();
	}

	HANDLE GetCurrentHandle()
	{
		return ::GetCurrentProcess();
	}

	std::vector<Pid> GetPidByClassNameOfWindow(constStrRef className)
	{
		//FindWindow(className,)
		return {};
	}

	Pid GetPidOfWindow(constStrRef className, constStrRef windowText)
	{
		auto hwnd = FindWindow(className.c_str(), windowText.c_str());
		return GetPidFromHwnd(hwnd);
	}

	std::vector<DWORD> GetPidsOfChildProcess(Pid parentPid)
	{
		std::vector<DWORD> pidsOfChild;
		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			return pidsOfChild;
		}
		Win32HandleGuard guard(hProcessSnap);
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
		return pidsOfChild;
	}

	///根据进程id获得伪句柄
	HANDLE GetHandleFromPid(Pid pid)
	{
		auto handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		if(INVALID_HANDLE_VALUE==handle)
			handle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
		return handle;
	}

	///根据进程句柄获得pid
	DWORD GetPidFromHandle(HANDLE handle)
	{
		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			return -1;
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
		return -2;
	}

	///暴力结束一个进程
	void ProcessInspector::KillProcess(HANDLE handle)
	{
		::TerminateProcess(handle, 1);
	}

	void ProcessInspector::KillProcess(Pid pid)
	{
		KillProcess(GetHandleFromPid(pid));
	}

	ProcessInfo ProcessInspector::GetProcessInfo(Pid processID)
	{
		auto hwndVec = FindHwndByPid(processID);
		auto hwnd = hwndVec[0];
		struct ProcessInfo process;
		wchar_t buffer[1024] = { 0 };
		if (hwnd)
		{
			::GetWindowText(hwnd, buffer, sizeof(buffer) / sizeof(buffer[0]));
			process.windowText = buffer;
			memset(buffer, 0, sizeof(buffer));
			::GetClassName(hwnd, buffer, sizeof(buffer) / sizeof(buffer[0]));
			process.className = buffer;
			memset(buffer, 0, sizeof(buffer));
			process.hWnd = hwnd;
			process.tid = GetWindowThreadProcessId(hwnd, &process.pid);
			process.isVisible = IsWindowVisible(hwnd);
		}
		GetProcessName(process.pid, buffer);
		process.exeName = buffer;
		return process;
	}

	bool GetProcessName(Pid processID, wchar_t* buffer)
	{
		HMODULE hMods[1024] = { 0 };
		DWORD cbNeeded = 0;
		unsigned int i = 0;
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			FALSE, processID);
		if (NULL == hProcess)
			return false;
		if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
		{
			::GetModuleFileNameEx(hProcess, hMods[i], buffer, sizeof(TCHAR) * MAX_PATH);
		}
		CloseHandle(hProcess);
		return true;
	}

	std::vector<HWND> FindHwndByPid(const Pid dwProcessId)
	{
		std::vector<HWND> re;
		HWND hWnd = GetTopWindow(NULL);
		while (hWnd)
		{
			DWORD id;
			GetWindowThreadProcessId(hWnd, &id);
			if (id == dwProcessId)
			{
				re.push_back(hWnd);
				hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
			}
			else
				hWnd = GetNextWindow(hWnd, GW_HWNDNEXT);
		}
		return re;
	}

	Pid GetPidFromHwnd(HWND hwnd)
	{
		DWORD pid = -1;
		::GetWindowThreadProcessId(hwnd, &pid);
		return pid;
	}

}
