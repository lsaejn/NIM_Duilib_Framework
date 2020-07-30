#pragma once

#include <windows.h>

#include <string>
#include <vector>

#include "processIterator.h"
#include "windowIterator.h"

namespace ProcessHelper
{
	using Pid = DWORD;

	///获取当前进程PID
	Pid GetCurrentPid();

	///根据进程句柄获得pid
	Pid GetPidFromHandle(HANDLE handle);

	///获取当前进程HANDLE
	HANDLE GetCurrentHandle();

	///根据进程id获得句柄
	HANDLE GetHandleFromPid(Pid pid);

	///结束一个进程
	void KillProcess(HANDLE handle);
	void KillProcess(Pid pid);
}
