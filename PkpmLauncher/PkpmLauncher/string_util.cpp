#include "pch.h"
#include "string_util.h"
#include "RapidjsonForward.h"
#include <string>

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
		static auto path = nbase::win32::GetCurrentModuleDirectory() + L"cfg/pkpm.ini";
		return path;
	}

	std::string FullPathOfPkpmIniA()
	{
		static auto path = nbase::UnicodeToAnsi(nbase::win32::GetCurrentModuleDirectory()
			+ L"cfg/pkpm.ini");
		return path;
	}

	std::string DictToJson(const std::vector<std::pair<std::string, std::string>>& dict)
	{
		rapidjson::Document doc;
		rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

		rapidjson::Value root(rapidjson::kObjectType);
		for (auto i = 0; i != dict.size(); ++i)
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
		nbase::StringReplaceAll("#",
			replace,
			result);
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

}

