#include<iostream>
#include <map>
#include <string>
#include <optional>
#include<windows.h>
using namespace std;


bool startWith(const char* src, const char* des)
{
	return (strlen(des) <= strlen(src)) &&
		(!memcmp(src, des, strlen(des)));
}

bool FilePathIsExist(const char* filepath_in, bool is_directory)
{
	const DWORD file_attr = ::GetFileAttributes(filepath_in);
	if (file_attr != INVALID_FILE_ATTRIBUTES)
	{
		if (is_directory)
			return (file_attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
		else
			return true;
	}
	return false;
}

///按照这篇文章改的https://blog.csdn.net/u014101410/article/details/77259886

bool CheckRegKey()
{
	HKEY cpp_key = NULL;
	LPCTSTR cpp_data = "SOFTWARE\\PKPM\\MAIN\\PATH";
	//@根表名称 @要打开的子表项;@固定值-0;@申请的权限;@返回句柄；
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, cpp_data, 0, KEY_READ, &cpp_key))
	{
		RegCloseKey(cpp_key);
		return true;
	}
	return false;
}

using KeyMap = std::map<std::string, std::string>;

 std::optional<KeyMap> read_value()
{
	 KeyMap result;
	 HKEY cpp_key = NULL;
	 //PVOID OldValue = NULL;
	 //Wow64DisableWow64FsRedirection(&OldValue);
	 wchar_t* cpp_data = L"SOFTWARE\\360Safe\\360Scan";
	 auto ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\PKPM\\PKPM-BIM建筑协同设计系统", 0, KEY_WOW64_64KEY | KEY_READ, &cpp_key);

	 if (ERROR_SUCCESS == RegOpenKeyExW(HKEY_LOCAL_MACHINE, cpp_data, 0, KEY_READ, &cpp_key))
	 {
		 DWORD KeyCnt = 0;
		 DWORD KeyMaxLen = 0;
		 DWORD NameCnt = 0;
		 DWORD NameMaxLen = 0;
		 DWORD MaxDateLen =0;
		 if (ERROR_SUCCESS == RegQueryInfoKey(cpp_key, NULL, NULL, 0, &KeyCnt, &KeyMaxLen, NULL, &NameCnt, &NameMaxLen, &MaxDateLen, NULL, NULL))
		 {
			 for (DWORD dwIndex = 0; dwIndex < NameCnt; dwIndex++)
			 {
				 DWORD DateSize = MaxDateLen + 1;
				 DWORD NameSize = NameMaxLen + 1;
				 DWORD Type = 0;
				char* szValueName = (char*)malloc(NameSize);
				LPBYTE szValueDate = (LPBYTE)malloc(DateSize);
				RegEnumValue(cpp_key, dwIndex, szValueName, &NameSize, NULL, &Type, szValueDate, &DateSize);
				if(startWith(szValueName, "CFGV"))
					result.insert({ szValueName, (char*)szValueDate });
			 }
		 }
		 else
		 {
			cout << "读取子键失败！" << endl;
		 }
	 }
	else
	 {
		cout << "打开注册表失败！" << endl;
	}
	if(cpp_key)
		RegCloseKey(cpp_key);
	if (result.empty())
		return {};
	return result;
}

 /*
 可以传入ini文件的绝对/相对路径。
 若命令行找不到，程序找当前exe目录下的OpenPKPM.ini
 程序依次尝试读取
 [AppPath]
 Path=....
 [PriorityVersion]
 Version=CFGV52
 */
 std::optional<std::string> hasConfigFile(int argc, char** argv)
 {
	 if(2 == argc)
		 return { argv[1]};
	 if (FilePathIsExist("OpenPKPM.ini", false))
		 return "OpenPKPM.ini";
	 return {};
 }

int main(int argc, char** argv)
{
	auto op= hasConfigFile(argc, argv);
	if (op.has_value())
	{
		std::string configFilePath = op.value();
		const int BufferSize = 1024;
		char buffer[BufferSize] = {0};
		GetPrivateProfileString("AppPath", "Path", "error", buffer, BufferSize, configFilePath.c_str());
		if (std::string(buffer) != "error")
		{
			memset(buffer, 0, BufferSize);
			GetPrivateProfileString("PriorityVersion", "Version", "error", buffer, BufferSize, configFilePath.c_str());
		}

	}


	bool hasPkpmmain=CheckRegKey();
	auto values=read_value();
	if (values.has_value())
	{
		
	}
	system("pause");
	return 0;
}