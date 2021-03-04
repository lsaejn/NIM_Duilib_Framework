#include "pch.h"

#include <fstream>

#include "Alime/FileSystem.h"

#include "ConfigFileManager.h"
#include "utility.h"
#include "SpdLogForward.h"

#pragma warning(disable: 4706)

//换肤的结果会被pkpmmain使用，所以最后还是决定把结果丢到pkpm.ini里
//方案是前端定的。 0-black 1-blue。我也不知道为什么搞这么奇怪

/*
为了让配置文件向前兼容，新增的字段都设有默认值，不抛出异常
*/

class PkpmConfigManager
{
public:

	static std::pair<int, std::wstring> GetGuiStyleInfo()
	{
		auto path = application_utily::FullPathOfPkpmIni();
		int styleIndex = GetPrivateProfileInt(L"InterfaceStyle", L"index", 0, path.c_str());
		wchar_t buffer[32] = { 0 };
		GetPrivateProfileStringW(L"InterfaceStyle", L"name", L"black", buffer, ArraySize(buffer), path.c_str());
		std::wstring styleName = buffer;
		return { styleIndex , styleName };
	}

	static void SetGuiStyleInfo(const std::pair<int, std::wstring>& info)
	{
		auto path = application_utily::FullPathOfPkpmIni();
		int ret = 0;
		ret = WritePrivateProfileString(L"InterfaceStyle", L"index", std::to_wstring(info.first).c_str(), path.c_str());
		//if(!ret)
		ret = WritePrivateProfileString(L"InterfaceStyle", L"name", info.second.c_str(), path.c_str());
		if (!ret)
		{
			MsgBox::Show(ui::MutiLanSupport::GetInstance()->GetStringViaID(L"ERROR_TIP_FAILED_TO_SAVE_STYLEINFO"), true);
		}
			
	}

};

void ConfigManager::CheckAdaptDpi()
{
	{
		std::ifstream ifs(nbase::UnicodeToAnsi(filePath_));
		if (!ifs.good())
			return;
		nlohmann::json json;
		ifs >> json;
		enableManualAdaptDpi_ = json[u8"enableAutoModifyWindow"];
		enableAdaptDpi_ = json[u8"enableAdaptDpi"];
	}
}

ConfigManager::ConfigManager()
	:enableManualAdaptDpi_(false),
	enableAdaptDpi_(false),
	deadline_(INT32_MAX),
	styleIndex_(0),
	folderDialogType_(0),
	enableStartPkpmmainDirect_(false),
	enableReadEnvFromConfig_(false),
	enableAcceptFileForAdmin_(false),
	enableDebugMode_(false),
	filePath_(nbase::win32::GetCurrentModuleDirectory()
		+ L"resources\\themes\\default\\defaultConfig.json")
{
	LoadConfigFile();
}


void ConfigManager::LoadConfigFile()
{
	//MsgBox::Show(L"Fatal error, fail to find config file", (std::wstring)L"defaultConfig.json");
	std::string content;
	bool succ = nbase::ReadFileToString(filePath_, content);
	if (!succ)
	{
		MsgBox::Show(L"Fatal error, fail to find config file", (std::wstring)L"defaultConfig.json");
		std::abort();
	}
	try
	{
		json_ = nlohmann::json::parse(content);
		auto& json = json_;
		enableManualAdaptDpi_ = json[u8"enableManualAdaptDpi"];
		enableAdaptDpi_ = json[u8"enableAdaptDpi"];
		enableWebPageRefresh_ = json[u8"enableRefresh"];
		defaultAdvertise_ = json[u8"defaultAdvertise"].dump();
		folderDialogType_ = json["folderDialogType"];
		server_ = nbase::UTF8ToUTF16(json["server"]);
		advertisementQuery_ = nbase::UTF8ToUTF16(json["query"]);
		cefFormWindowText_ = nbase::UTF8ToUTF16(json["windowText"]);
		cefFormClassName_ = nbase::UTF8ToUTF16(json["className"]);
		relativePathForHtmlRes_ = nbase::UTF8ToUTF16(json["relativePathForHtmlRes"]);
		skinFile_ = nbase::UTF8ToUTF16(json["skinFile"]);
		skinFolder_ = nbase::UTF8ToUTF16(json["skinFolder"]);
		nativeArticlesPath_ = nbase::UTF8ToUTF16(json["nativeArticles"]);
		webArticlesPath_ = nbase::UTF8ToUTF16(json["webArticles"]);
		launchDllName_= nbase::UTF8ToUTF16(json["launchDllName"]);
		installerPath_= nbase::UTF8ToUTF16(json["installer"]);
		deadline_ = json["deadline"];
		deadline_ = deadline_ <= 0 ? 7 : deadline_;
		bimWebUrl_ = nbase::UTF8ToUTF16(json["bimWebUrl"]);
		enableStartPkpmmainDirect_= json[u8"lauchDirectly"];
		languageFileName_ = nbase::UTF8ToUTF16(json[u8"languageFile"]);
		//以下为后续内容，为了保证exe兼容所有配置文件，暂时这么处理
		//历史负担重的公司是这样的。
		if (json_.contains("enableEnv")&& (enableReadEnvFromConfig_ = json_[u8"enableEnv"]))
		{
				nlohmann::json envArray = json_["envs"];
				for (auto elem : envArray)
					envs_.push_back(nbase::UTF8ToUTF16(elem));
		}
		if (json_.contains("acceptFileForAdmin"))
		{
			enableAcceptFileForAdmin_ = json[u8"acceptFileForAdmin"];
		}
		if ( json_.contains("debugMode"))
		{
			enableDebugMode_ = json["debugMode"];
		}
		/*
			std::wstring structWebUrl_;
	std::wstring integrityCheckExePath_;
	std::wstring pManagerExePath_;
	std::wstring regControlExePath_;
	std::wstring authorizeExePath_;
		*/
		if (json_.contains("structWebUrl"))
		{
			structWebUrl_= nbase::UTF8ToUTF16(json["structWebUrl"]);
		}
		if (json_.contains("programPaths"))
		{
			auto& pathObj = json_["programPaths"];
			for (const auto& e : pathObj)
			{
				programPaths_[nbase::UTF8ToUTF16(e["name"])] =
					nbase::win32::GetCurrentModuleDirectory()+nbase::UTF8ToUTF16(e["path"]);
			}
		}
	}
	catch (std::exception& )//parse excetion
	{
		MsgBox::Show(L"Fatal error, fail to parse config file", (std::wstring)L"defaultConfig.json");
		std::abort();
	}
}

std::wstring ConfigManager::GetStructDownLoadWeb() const
{
	return structWebUrl_;
}

std::wstring ConfigManager::GetProgramPath(const std::wstring& key) const
{
	std::wstring ret;
	const auto iter = programPaths_.find(key);
	if(iter!= programPaths_.end())
	{
		ret =iter->second;
	}
	return ret;
}

//bool ConfigManager::IsSystemFolderDialogOn() const
//{
//	return systemFolderSelection_;
//}

bool ConfigManager::IsWebPageRefreshOn() const
{
	return enableWebPageRefresh_;
}

bool ConfigManager::isStartPkpmmainDirect() const
{
	return enableStartPkpmmainDirect_;
}

bool ConfigManager::IsDebugModeOn() const
{
	return enableDebugMode_;
}

bool ConfigManager::CanReadEnvFromConfig() const
{
	return enableReadEnvFromConfig_;
}

bool ConfigManager::IsAcceptFileForAdminOn() const
{
	return enableAcceptFileForAdmin_;
}

std::string ConfigManager::GetDefaultAdvertise() const
{
	return defaultAdvertise_;
}

std::wstring ConfigManager::GetAdvertisementServer() const
{
	return server_;
}

std::wstring ConfigManager::GetAdvertisementQuery() const
{
	return advertisementQuery_;
}

std::wstring ConfigManager::GetCefFormWindowText() const
{
	return cefFormWindowText_;
}

std::wstring ConfigManager::GetCefFormClassName() const
{
	return cefFormClassName_;
}

std::wstring ConfigManager::GetRelativePathForHtmlRes() const
{
	return relativePathForHtmlRes_;
}

std::wstring ConfigManager::GetSkinFile() const
{
	return skinFile_;
}
std::wstring ConfigManager::GetSkinFolderPath() const
{
	return skinFolder_;
}

std::wstring ConfigManager::GetNativeArticlePath() const
{
	return nativeArticlesPath_;
}

std::wstring ConfigManager::GetWebArticlePath() const
{
	return webArticlesPath_;
}

std::wstring ConfigManager::GetInstallerPath() const
{
	return installerPath_;
}

std::wstring ConfigManager::GetLanguageFile() const
{
	return languageFileName_;
}

bool ConfigManager::IsAdaptDpiOn() const
{
	return enableAdaptDpi_;
}

bool ConfigManager::IsModifyWindowOn() const
{
	return enableManualAdaptDpi_;
}

int32_t ConfigManager::DaysLeftToNotify() const
{
	return deadline_;
}

std::wstring ConfigManager::GetLaunchDllName() const
{
	return launchDllName_;
}

std::wstring ConfigManager::GetBimDownLoadWeb() const
{
	return bimWebUrl_;
}

const std::vector<std::wstring>& ConfigManager::GetEnvPaths() const
{
	return envs_;
}

int32_t ConfigManager::GetFolderDialogType() const
{
	return folderDialogType_;
}

std::pair<int, std::wstring> ConfigManager::GetGuiStyleInfo() const
{
	return PkpmConfigManager::GetGuiStyleInfo();
}

//前端没有说明为什么需要两个变量来标识主题
void ConfigManager::SetGuiStyleInfo(const std::pair<int, std::wstring>& info)
{
	return PkpmConfigManager::SetGuiStyleInfo(info);
}