#pragma once
#include <string>
#include <string_view>

#include "windows.h"

#include "FileSystem.h"
#include "util.h"
#include <regex>
#include "assert.h"


///注册表函数是按照这篇文章改的https://blog.csdn.net/u014101410/article/details/77259886
/// 
class Regedit
{
public:
	using KeyMap = std::map<std::string, std::string>;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="key">CFG键值</param>
	/// <returns>CFG路径</returns>
	std::string ReadSpecificKey(std::string_view key)
	{
		auto op = read_value();
		if (op.has_value())
		{
			KeyMap map = op.value();
			if (map.find(key.data()) != map.end())
			{
				return map[key.data()];
			}
		}
		return {};
	}

	std::string ReadLatestKey()
	{
		auto op=read_value();
		if (op.has_value())
		{
			KeyMap map = op.value();
			assert(map.size() > 0);
			auto path = map.rbegin()->second;
			return path;
		}
		return {};
	}

private:
	std::optional<KeyMap> read_value()
	{
		KeyMap result;
		HKEY cpp_key = NULL;
		char* cpp_data = "SOFTWARE\\PKPM\\MAIN\\PATH";
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, cpp_data, 0, KEY_READ, &cpp_key))
		{
			DWORD KeyCnt = 0;
			DWORD KeyMaxLen = 0;
			DWORD NameCnt = 0;
			DWORD NameMaxLen = 0;
			DWORD MaxDateLen = 0;
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
					if (util::startWith(szValueName, "CFGV"))
						result.insert({ szValueName, (char*)szValueDate });
				}
			}
			else
			{
				std::cout << "读取PKPMPATH失败，本机没有安装PKPMMAIN！" << std::endl;
			}
		}
		else
		{
			std::cout << "打开注册表失败！" << std::endl;
		}
		if (cpp_key)
			RegCloseKey(cpp_key);
		if (result.empty())
			return {};
		return result;
	}

};

class ConfigFile
{
public:
	ConfigFile(const std::string& filename)
		:filePath_(filename)
	{
	}

	std::string ReadString(std::string_view section, std::string_view key)
	{
		const int BufferSize = 2048;
		char buffer[BufferSize] = { 0 };
		GetPrivateProfileString(section.data(), key.data(), "error", buffer, BufferSize, filePath_.c_str());
		return buffer;
	}

private:
	const std::string& filePath_;
};

class Launcher
{
public:
	Launcher()
	{
	}

	Launcher(std::string_view filePath)
		:filePath_(filePath)
	{
	}

	void OpenPKPMWithConfigFile()
	{
		ConfigFile file(filePath_);
		std::string pkpmmainPath;
		pkpmmainPath = file.ReadString("AppInfo", "Path");
		if (pkpmmainPath != "error")
		{
			bool ret = LaunchAppByPath(pkpmmainPath);
			if (ret)
				return;
			std::cout << "AppInfo-Path打开程序失败, 路径: " << pkpmmainPath << std::endl;
		}
		std::string cfgKey = file.ReadString("AppInfo", "PriorityVersion");
		if (cfgKey != "error")
		{
			bool ret=LaunchAppByRegedit(cfgKey);
			if (ret)
				return;
			std::cout << "AppInfo-PriorityVersion打开程序失败, 路径: " << cfgKey << std::endl;
		}
		//为了省事0.0
		std::cout << "配置文件错误, 尝试使用注册表读取程序路径" << std::endl;
		return OpenPKPMNoConfigFile();
	}

	void OpenPKPMNoConfigFile()
	{
		bool ret = LaunchAppByRegedit("");
		if (!ret)
			LaunchAppByWeb();
	}

private:
	bool FindLatestCfgKey(std::string& cfgKey)
	{
		return false;
	}

	bool LaunchAppByPath(std::string& path)
	{
		path=util::trim(std::move(path));
		if (fs::FilePathIsExist(path.c_str(), false))
		{
			fs::CreateProcessWithCommand(path.c_str(), "", NULL);
			return true;
		}
		else return false;
	}

	bool LaunchAppByRegedit(const std::string& regKey)
	{
		Regedit launcher;
		std::string cfgFolder;//注册表里写的是cfg的路径，为的怕是方便stsSect去读吧, 小作坊式的公司0.0
		if (regKey.empty())
		{
			cfgFolder = launcher.ReadLatestKey();
		}
		else
		{
			cfgFolder = launcher.ReadSpecificKey(regKey);
		}
		if (cfgFolder.empty())
		{
			std::cout << "无法通过注册表找到CFG路径" << std::endl;
			return false;
		}
		auto indexOfDot=cfgFolder.rfind("\\");
		// string_view will cause a bug, as we call util::FindFiles(folder.data(),...)
		std::string root(cfgFolder.data(), indexOfDot+1);
		auto filename=FindPkpmLauncherInThisFolder(root);
		return LaunchAppByPath(root.data()+ filename);
	}

	void LaunchAppByWeb()
	{
		ShellExecute(NULL, ("open"), "https://www.pkpm.cn/index.php?m=content&c=index&a=lists&catid=70", NULL, NULL, SW_SHOW);
	}

	std::string FindPkpmLauncherInThisFolder(std::string_view folder)
	{
		//难道...这帮人连自己程序的大小写也可能弄错?
		auto vec=util::FindFiles(folder.data(), "PKPM", "exe");
		decltype(vec) copy;
		for (size_t i = 0; i != vec.size(); ++i)
		{
			//鬼知道你他妈的有什么黑历史
			std::regex rgx(R"(PkPm[\d]+V[\d]+\.exe)", std::regex_constants::icase);
			bool ret=std::regex_match(vec[i], rgx);
			if (ret)
				copy.push_back(vec[i]);
		}
		if (copy.size() != 1)
		{
			std::cout << "目录下没有启动程序，正在启动多版本" << std::endl;
			return "多版本.exe";
		}
		else
			return copy[0];
	}

	std::string filePath_;//配置文件名
};
