#pragma once
#ifndef PROCESSINFO_H_
#define PROCESSINFO_H_

#include <string>
#include "string_utility.h"

namespace ProcessInfo
{
	//fix me
	std::string hostName();
	std::string userName();
	std::string exeName();
	std::string exePath();
	DWORD getpid();
	DWORD gettid();
	DWORD getppid();
	BOOL GetParentProcessName(TCHAR* processName);
	BOOL GetParentProcessId(DWORD* parent_process_id);
	BOOL GetProcessNameByPid(DWORD processID, TCHAR *outString, size_t sizeOfOutString);
	std::string  getWindowsVersion(const int mv, const int vv);
	BOOL CheckWindowsVersion(int* mainVersion, int* viceVersion, const char* filename = "kernel32.dll");
}

class StartupChecks
{
public:
	static const StartupChecks& instance()
	{
		static StartupChecks ins;
		return ins;
	}

	bool isStartupDefined() const;
	bool inheritFrom(const char* firstAppNameL) const;
	//template<typename T, typename... Args>
	//bool StartupChecks::inheritFromThese(const T& t, Args... args)
	//{
	//	return inheritFrom(t)||inheritFrom(args...);
	//}
	//template<typename T>
	//bool StartupChecks::inheritFromThese(const T& t)
	//{
	//	return inheritFrom(t);
	//}
private:
	StartupChecks(){}
	StartupChecks(const StartupChecks&);
	StartupChecks operator=(const StartupChecks& );
};


#endif

