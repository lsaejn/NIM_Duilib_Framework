#pragma once
#include "windows.h"

#include <string>

namespace fs
{
	static bool FilePathIsExist(const std::string& filepath_in, bool is_directory)
	{
		const DWORD file_attr = ::GetFileAttributes(filepath_in.c_str());
		if (file_attr != INVALID_FILE_ATTRIBUTES)
		{
			if (is_directory)
				return (file_attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
			else
				return true;
		}
		return false;
	}

	static std::string GetAppPath()
	{
		char buffer[1024 * 4];
		GetModuleFileName(NULL, buffer, sizeof(buffer) / sizeof(*buffer));
		int pos = -1;
		int index = 0;
		while (buffer[index])
		{
			if (buffer[index] == L'\\' || buffer[index] == L'/')
				pos = index;
			index++;
		}
		return std::string(buffer, pos + 1);
	}

	static bool CreateProcessWithCommand(const char* application, const char* command, HANDLE* process)
	{
		PROCESS_INFORMATION pi;
		STARTUPINFO si;

		memset(&si, 0, sizeof(si));
		si.dwFlags = 0;
		si.cb = sizeof(si);
		si.hStdInput = NULL;
		si.hStdOutput = NULL;
		si.hStdError = NULL;

		char* command_dup = _strdup(command);

		if (::CreateProcess(application,
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

}
