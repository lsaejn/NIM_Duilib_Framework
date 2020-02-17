#include "stdafx.h"
#include "ProcessInfo.h"
#include <tlhelp32.h>//for LPPROCESSENTRY32
#include "psapi.h"
#include "strsafe.h"//for StringCbCopy
#include <tchar.h>//for _tslen

#pragma warning(disable:4996)
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Version.lib")


namespace ProcessInfo
{
BOOL FindProcessID(HANDLE snap, DWORD id, LPPROCESSENTRY32 ppe)
{
	ppe->dwSize = sizeof(PROCESSENTRY32);
	BOOL res = Process32First(snap, ppe);
	while (res)
	{
		if (ppe->th32ProcessID == id)
			return TRUE;
		res = Process32Next(snap, ppe);
	}
	return FALSE;
}

/*
虽然有id，但是父进程关闭了，导致查不到进程名字
似乎应该判断父进程是不是explore.exe。我不确定
*/
BOOL GetProcessNameByPid(DWORD processID, TCHAR *outString, size_t sizeOfOutString)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if(!hProcess )
	{
		const char* AppName="PKPMMAINLAUNCHER.exe";
		memcpy(outString,AppName,sizeOfOutString);
		return TRUE;
	}
	TCHAR processName[MAX_PATH] = { 0 };
	GetModuleFileNameEx(hProcess, NULL, processName, MAX_PATH);
	if (GetLastError() == ERROR_SUCCESS)
	{
		StringCbCopyA(outString, sizeOfOutString, processName);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL GetParentProcessId(DWORD* parent_process_id)
{
	HANDLE hSnap;
	PROCESSENTRY32 pe;
	DWORD current_pid = GetCurrentProcessId();
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnap)
		return FALSE;
	if (!FindProcessID(hSnap, current_pid, &pe))
		return FALSE;
	if (!CloseHandle(hSnap))
		return FALSE;
	*parent_process_id = pe.th32ParentProcessID;
	return TRUE;
}

BOOL GetParentProcessName(TCHAR* processName)
{
	DWORD pid = 0;
	if (FALSE == GetParentProcessId(&pid))
		return FALSE;
	TCHAR str[260] = { 0 };
	//MessageBox(NULL,"click this","ooo",1);
	GetProcessNameByPid(pid, str,sizeof(str));
	//MessageBox(NULL,str,"ooo",1);
	char pidName[256]={0};
	itoa(pid,pidName,10);
	//MessageBox(NULL,pidName,"ooo",1);
	for (size_t i = 0; i != _tcslen(str); ++i)
	{
		if (str[_tcslen(str) - 1 - i] == _T('\\'))
		{
			//fix me ,size写错了0.0
			memcpy(processName, str + _tcslen(str) - i, MAX_PATH);
			return TRUE;
		}
	}
	memcpy(processName, str, _tcslen(str));
	return TRUE;
}

BOOL CheckWindowsVersion(int* mainVersion, int* viceVersion, const char* filename)
{
	//Call the GetFileVersionInfoSize function before calling the GetFileVersionInfo function
	DWORD infoSize = GetFileVersionInfoSize(filename, NULL);
	if (infoSize > 0)
	{
		char* buffer = new char[infoSize];
		if (GetFileVersionInfo(filename, NULL, infoSize, buffer))
		{
			VS_FIXEDFILEINFO* info = NULL;
			auto infoSize = sizeof(VS_FIXEDFILEINFO);
			if(VerQueryValue(buffer,_T("\\"),(VOID**)(&info),&infoSize))
			{
				//osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1
				*mainVersion = HIWORD(info->dwFileVersionMS);
				*viceVersion = HIWORD(info->dwFileVersionLS);
				//MessageBox(NULL, getWindowsVersion(*mainVersion, *viceVersion).c_str(), "version", 1);
				return TRUE;
			}
		}
	}
	return FALSE;
}

std::string  getWindowsVersion(const int mv, const int vv)
{
	//https://docs.microsoft.com/zh-cn/windows/win32/sysinfo/operating-system-version
	/*
	windows版本很乱0.0
	*/
	if (mv == 5 && (vv == 1 || vv == 2))
		return "Windows XP";
	else if (mv == 6 && vv == 1)
		return	"Windows 7";
	else if (mv == 6 && vv > 1)
		return "Windows 8";
	else if (mv == 10)
		return "Windows 10";
	return "";
}
}

bool StartupChecks::isStartupDefined() const
{
	char dlgSwitch[256]={0};
	CString strPath;
	DWORD dwSize = MAX_PATH;
	::GetModuleFileName(NULL, strPath.GetBuffer(MAX_PATH), dwSize);
	strPath.ReleaseBuffer(dwSize);
	CString Path= strPath.Left(strPath.ReverseFind(_T('\\'))+1);
	Path+="..\\CFG\\PKPM.ini";
	GetPrivateProfileStringA("HTML", "EnableHtmlDialog", "false", dlgSwitch, 256, Path);
	return string_utility::StringPiece(dlgSwitch)=="true";
}

bool StartupChecks::inheritFrom(const char* firstAppNameL) const
{
	using string_utility::StringPiece;
	char parentName[260]={0};
	auto ret=ProcessInfo::GetParentProcessName(parentName);
	if(ret)
	{
		if(StringPiece(parentName)==StringPiece(firstAppNameL))
			return true;	
	}
	return false;
}



