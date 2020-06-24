#include "pch.h"

#include <string>

#include "RapidjsonForward.h"

#include "string_util.h"
#include "templates.h"
#include "RegKeyRW.h"


namespace string_utility
{
	std::vector<std::string> string_split(const std::string& s, std::string&& c)
	{
		std::vector<std::string> result;
		size_t len = s.length();
		std::string::size_type pos1 = 0;
		std::string::size_type pos2 = s.find(c);	
		while (std::string::npos != pos2)
		{
			result.push_back(s.substr(pos1, pos2 - pos1));
			pos1 = pos2 + c.size();
			pos2 = s.find(c, pos1);
		}
		if (pos1 != len)
			result.push_back(s.substr(pos1));
		return result;
	}


	bool startWith(const char* src, const char* des)
	{
		return (strlen(des) <= strlen(src)) && 
			(!memcmp(src,des,strlen(des)) );
	}

	bool endWith(const char* src, const char* des)
	{
		int offset=strlen(src) - strlen(des);
		return  offset>=0
			&& startWith(src+offset, des);
	}

	std::string replace_all(std::string&& s, const std::string& search, const std::string& rep)
	{
		size_t pos = 0;
		while ((pos = s.find(search, pos)) != std::string::npos)
		{
			s.replace(pos, search.size(), rep);
			pos += rep.size();
		}
		return std::move(s);
	}
}

namespace application_utily
{
	std::wstring FullPathOfPkpmIni()
	{
		static auto path = nbase::win32::GetCurrentModuleDirectory() + L"cfg\\pkpm.ini";
		return path;
	}

	std::string FullPathOfPkpmIniA()
	{
		static auto path = nbase::UnicodeToAnsi(FullPathOfPkpmIni());
		return path;
	}

	std::string DictToJson(const std::vector<std::pair<std::string, std::string>>& dict)
	{
		rapidjson::Document doc;
		rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

		rapidjson::Value root(rapidjson::kObjectType);
		for (size_t i = 0; i != dict.size(); ++i)
		{
			rapidjson::Value strObject(rapidjson::kStringType);
			strObject.SetString(dict[i].first.c_str(), allocator);
			rapidjson::Value strObjectValue(rapidjson::kStringType);
			strObjectValue.SetString(dict[i].second.c_str(), allocator);
			root.AddMember(strObject, strObjectValue, allocator);
		}
		rapidjson::StringBuffer s;
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		root.Accept(writer);
		std::string result = s.GetString();
		return s.GetString();
	}

	//for file: protocol
	//测试的特殊字符串: c%c! - 副本@~`$()+-=_][{}';
	//img src中包含特殊字符时，cef 处理不了#
	std::string FileEncode(const std::string& query)
	{
		std::string result = query;
		std::string replace = "%";
		replace += "0123456789ABCDEF"['#' / 16];
		replace += "0123456789ABCDEF"['#' % 16];
		nbase::StringReplaceAll("#", replace, result);
		return result;
	}

	void OpenDocument(const std::wstring& filePath)
	{
		SHELLEXECUTEINFO ShExecInfo;
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = _T("open");
		ShExecInfo.lpFile = filePath.c_str();
		ShExecInfo.lpParameters = _T("");
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_SHOW;
		ShExecInfo.hInstApp = NULL;
		ShellExecuteEx(&ShExecInfo);
	}

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

	void GodBlessThisProcess(wchar_t* god)
	{
		wchar_t buffer[256] = { 0 };
		auto param = L" " + std::to_wstring(::GetCurrentProcessId());
		memcpy_s(buffer, 256 * sizeof(wchar_t), param.c_str(), param.length() * sizeof(wchar_t));
		auto pathOfGodWindow = nbase::win32::GetCurrentModuleDirectory() + god;
		HANDLE handleOfGuard= INVALID_HANDLE_VALUE;
		application_utily::CreateProcessWithCommand(pathOfGodWindow.c_str(), buffer, &handleOfGuard);
		if (handleOfGuard == INVALID_HANDLE_VALUE)
		{
			//LOG_ERROR<<
			return;
		}
		CloseHandle(handleOfGuard);
	}

	//not thread safe
	int setenv(const char* name, const char* value, int overwrite) 
	{
		if (overwrite == 0 && getenv(name) != nullptr)
		{
			return 0;
		}

		if (*value != '\0')
		{
			auto e = _putenv_s(name, value);
			if (e != 0)
			{
				errno = e;
				return -1;
			}
			return 0;
		}

		if (_putenv_s(name, "  ") != 0)
		{
			errno = EINVAL;
			return -1;
		}
		*getenv(name) = '\0';
		*(getenv(name) + 1) = '=';
		if (_wenviron != nullptr)
		{
			wchar_t buf[_MAX_ENV + 1];
			size_t len;
			if (mbstowcs_s(&len, buf, _MAX_ENV + 1, name, _MAX_ENV) != 0) {
				errno = EINVAL;
				return -1;
			}
			*_wgetenv(buf) = u'\0';
			*(_wgetenv(buf) + 1) = u'=';
		}

		if (!SetEnvironmentVariableA(name, value)) {
			errno = EINVAL;
			return -1;
		}
		return 0;
	}

	char* realpath(const char* path, char* resolved_path)
	{
		return _fullpath(resolved_path, path, _MAX_PATH);
	}

	std::wstring GetExePath()
	{
		wchar_t buffer[65536];
		GetModuleFileName(NULL, buffer, sizeof(buffer) / sizeof(*buffer));
		int pos = -1;
		int index = 0;
		while (buffer[index])
		{
			if (buffer[index] == L'\\' || buffer[index] == L'/')
			{
				pos = index;
			}
			index++;
		}
		return std::wstring(buffer, pos + 1);
	}

	bool GetClipBoardInfo(HWND wnd, std::string& filePath)
	{
		if (OpenClipboard(wnd))
		{
			ScopeFunc guard([]() {CloseClipboard();});
			if (IsClipboardFormatAvailable(CF_TEXT))
			{
				HANDLE hclip =INVALID_HANDLE_VALUE;
				hclip=GetClipboardData(CF_TEXT);
				if (hclip == INVALID_HANDLE_VALUE) 
					return false;
				char* pBuf = static_cast<char*>(GlobalLock(hclip));
				GlobalUnlock(hclip);
				filePath = std::move(std::string(pBuf));
				return true;
			}	
		}
		return false;
	}

	void OpenBimExe()
	{
		LPCTSTR cpp_data = L"SOFTWARE\\PKPM\\PKPM-BIM建筑协同设计系统";
		HKEY hKey = NULL;
		//@根表名称 @要打开的子表项;@固定值-0;@申请的权限;@返回句柄；
		if (ERROR_SUCCESS != RegOpenKeyExW(HKEY_LOCAL_MACHINE, cpp_data, 0, KEY_READ| KEY_WOW64_64KEY, &hKey))
			return;
		DWORD KeyCnt = 0;
		DWORD KeyMaxLen = 0;
		DWORD NameCnt = 0;
		DWORD NameMaxLen = 0;
		DWORD MaxDateLen = 0;
		std::wstring bimPath;
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
				if (std::wstring(szValueName) == L"PATH")
				{
					bimPath = (wchar_t*)(szValueDate);
					delete szValueName;
					break;
				}			
				delete szValueName;
			}
		}
		if (!bimPath.empty())
		{
			CreateProcessWithCommand(bimPath.c_str(), NULL, NULL);
		}
		else
		{
			ShellExecute(NULL, _T("open"), L"https://www.pkpm.cn/index.php?m=content&c=index&a=lists&catid=69", NULL, NULL, SW_SHOW);
		}
	}

}

