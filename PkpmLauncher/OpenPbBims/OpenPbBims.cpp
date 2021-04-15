// OpenPbBims.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "windows.h"
#include "Shlwapi.h"

#pragma comment(lib, "Shlwapi.lib")

bool CreateProcessWithCommand(const wchar_t* application, const wchar_t* command, HANDLE* process)
{
	PROCESS_INFORMATION pi;
	STARTUPINFOW si;

	memset(&si, 0, sizeof(si));
	si.dwFlags = 0;
	si.cb = sizeof(si);
	si.hStdInput = NULL;
	si.hStdOutput = NULL;
	si.hStdError = NULL;

	wchar_t* command_dup = wcsdup(command);

	if (::CreateProcessW(application,
		command_dup,
		NULL,
		NULL,
		(si.dwFlags & STARTF_USESTDHANDLES) ? TRUE : FALSE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&si,
		&pi))
	{
		::CloseHandle(pi.hThread);
		if (process == NULL)
			::CloseHandle(pi.hProcess);
		else
			*process = pi.hProcess;
		free(command_dup);
		return true;
	}

	free(command_dup);
	return false;
}

bool FindBimExe(std::wstring& bimPath)
{
	bimPath = L"";
	LPCTSTR cpp_data = L"SOFTWARE\\PKPM\\PKPM-BIMS";
	HKEY hKey = NULL;
	//@根表名称 @要打开的子表项;@固定值-0;@申请的权限;@返回句柄；
	if (ERROR_SUCCESS != RegOpenKeyExW(HKEY_LOCAL_MACHINE, cpp_data, 0, KEY_READ | KEY_WOW64_64KEY, &hKey))
	{
		return false;
	}
	DWORD KeyCnt = 0;
	DWORD KeyMaxLen = 0;
	DWORD NameCnt = 0;
	DWORD NameMaxLen = 0;
	DWORD MaxDateLen = 0;
	if (ERROR_SUCCESS == RegQueryInfoKey(hKey, NULL, NULL, 0, &KeyCnt, &KeyMaxLen, NULL, &NameCnt, &NameMaxLen, &MaxDateLen, NULL, NULL))
	{
		for (DWORD dwIndex = 0; dwIndex < NameCnt; dwIndex++)
		{
			DWORD DateSize = MaxDateLen + 1;
			DWORD NameSize = NameMaxLen + 1;
			DWORD Type = 0;
			wchar_t* szValueName = new wchar_t[NameSize];
			LPBYTE szValueDate = (LPBYTE)malloc(DateSize);
			RegEnumValue(hKey, dwIndex, szValueName, &NameSize, NULL, &Type, szValueDate, &DateSize);
			if (std::wstring(szValueName) == L"PATH" || std::wstring(szValueName) == L"Path")
			{
				bimPath = (wchar_t*)(szValueDate);
				bimPath += L"PBBim.exe";
				delete szValueName;
				if (!PathFileExists(bimPath.c_str()))
				{
					bimPath.clear();
					return false;
				}
				break;
			}
			delete szValueName;
		}
	}
	return !bimPath.empty();
}

//懒得改了
bool OpenBimExe(const std::wstring& bimPath)
{
	if (!bimPath.empty())
		return CreateProcessWithCommand(bimPath.c_str(), NULL, NULL);
	return false;
}

void DoOpenBim()
{
	std::wstring bimPath;
	if (FindBimExe(bimPath) && OpenBimExe(bimPath))
		;
	else
	{
		ShellExecute(NULL, L"open", L"https://www.pkpm.cn/index.php?m=content&c=index&a=lists&catid=69", NULL, NULL, SW_SHOW);
	}
}

int main()
{
	DoOpenBim();
	return 0;
}
