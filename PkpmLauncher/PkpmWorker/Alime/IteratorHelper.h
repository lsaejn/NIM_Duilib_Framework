#pragma once

#include <windows.h>

#include <string>
#include <vector>

#include "processIterator.h"
#include "windowIterator.h"

namespace ProcessHelper
{
	using Pid = DWORD;

	///��ȡ��ǰ����PID
	Pid GetCurrentPid();

	///���ݽ��̾�����pid
	Pid GetPidFromHandle(HANDLE handle);

	///��ȡ��ǰ����HANDLE
	HANDLE GetCurrentHandle();

	///���ݽ���id��þ��
	HANDLE GetHandleFromPid(Pid pid);

	///����һ������
	void KillProcess(HANDLE handle);
	void KillProcess(Pid pid);
}
