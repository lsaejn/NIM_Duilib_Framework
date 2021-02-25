#pragma once
#include <vector>
#include <string>


template<typename Ty, size_t nSize>
constexpr size_t ArraySize(Ty(&Arg)[nSize])
{
	(void)Arg;
	return nSize;
}

//a static func, so we won't freelibrary handle
#define USE_SYSTEM_API(NAME, API) \
static auto proc_##API = (decltype(&API))(GetProcAddress(GetModuleHandleW(L#NAME), #API))

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

	std::wstring GetExeFolderPath();
	bool GetClipBoardInfo(HWND wnd, std::string& filePath);
	bool OnOpenAutoTest();
	bool FindBimExe(std::wstring& resultPath);
	//打开Bim软件没有放到exe里，这是因为该软件的启动并不打算双击启动(尽管我支持了这样做)
	bool OpenBimExe(const std::wstring& bimPath);
	void OnOpenModelViewerMaster();//图模大师PKPM_Model_Master
	std::wstring CorrectCefUrl(const std::wstring&);
	bool IsOpenedWithAdminAccess(int pid=-1);

	static std::wstring errnoStr_(DWORD e)
	{
		LPTSTR buf;
		std::wstring result=L" unknow error";
		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS
			| FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
			NULL,
			e,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&buf,
			0,
			NULL))
		{
			result = buf;
			LocalFree(buf);
		}
		return result;
	}
}


using ClosurePkg = std::function<void()>;
class ScopeGuard
{
public:
	ScopeGuard(ClosurePkg func)
		:func_(std::move(func))
	{

	}

	~ScopeGuard()
	{
		func_();
	}
	ClosurePkg func_;
};