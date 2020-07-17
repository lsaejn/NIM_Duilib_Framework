#pragma once
#include <vector>
#include <string>


template<typename Ty, size_t nSize>
constexpr size_t ArraySize(Ty(&Arg)[nSize])
{
	(void)Arg;
	return nSize;
}
namespace string_utility
{
	bool startWith(const char* src, const char* des);
	bool endWith(const char* src, const char* des);
	std::vector<std::string> string_split(const std::string& s, std::string&& c);
}

namespace application_utily
{	
	std::wstring FullPathOfPkpmIni();
	std::string FullPathOfPkpmIniA();
	std::string DictToJson(const std::vector<std::pair<std::string, std::string>>& dict);
	std::string FileEncode(const std::string& query);
	void OpenDocument(const std::wstring& filePath);
	bool CreateProcessWithCommand(const wchar_t* application, const wchar_t* command, HANDLE* process);
	void GodBlessThisProcess(wchar_t* god= L"ProcessGuard.exe");
	int setenv(const char* name, const char* value, int overwrite);
	char* realpath(const char* path, char* resolved_path);
	std::wstring GetExePath();
	bool GetClipBoardInfo(HWND wnd, std::string& filePath);
	bool FindBimExe(std::wstring& resultPath);
	bool OpenBimExe(const std::wstring& bimPath);
	void OnOpenModelViewerMaster();//Í¼Ä£´óÊ¦PKPM_Model_Master
}