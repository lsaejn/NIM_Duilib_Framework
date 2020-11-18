#include "pch.h"

#include <fstream>

#include "Alime/FileSystem.h"

#include "ConfigFileManager.h"
#include "utility.h"


//�����Ľ���ᱻpkpmmainʹ�ã���������Ǿ����ѽ������pkpm.ini��
//������ǰ�˶��ġ� 0-black 1-blue����Ҳ��֪��Ϊʲô����ô���
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
		ret = WritePrivateProfileString(L"InterfaceStyle", L"name", info.second.c_str(), path.c_str());
		if (!ret)
			MsgBox::Show(L"�޷����滻����Ϣ", true);
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
		isManualAdaptDpiOn_ = json[u8"enableAutoModifyWindow"];
		isAdaptDpiOn_ = json[u8"enableAdaptDpi"];
	}
}

ConfigManager::ConfigManager()
	:isManualAdaptDpiOn_(false),
	isAdaptDpiOn_(false),
	deadline_(INT32_MAX),
	styleIndex_(0),
	folderDialogType_(0),
	isStartPkpmmainDirect_(false),
	filePath_(nbase::win32::GetCurrentModuleDirectory()
		+ L"resources\\themes\\default\\defaultConfig.json")
{
	LoadConfigFile();
}


void ConfigManager::LoadConfigFile()
{
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
		isManualAdaptDpiOn_ = json[u8"enableManualAdaptDpi"];
		isAdaptDpiOn_ = json[u8"enableAdaptDpi"];
		isWebPageRefreshOn_ = json[u8"enableRefresh"];
		systemFolderSelection_ = json[u8"systemFolderSelection"];
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
		isStartPkpmmainDirect_= json[u8"lauchDirectly"];
	}catch (...){
		MsgBox::Show(L"Fatal error, fail to parse config file", (std::wstring)L"defaultConfig.json");
		std::abort();
	}
}

bool ConfigManager::IsSystemFolderDialogOn() const
{
	return systemFolderSelection_;
}

bool ConfigManager::IsWebPageRefreshOn() const
{
	return isWebPageRefreshOn_;
}

bool ConfigManager::isStartPkpmmainDirect() const
{
	return isStartPkpmmainDirect_;
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

bool ConfigManager::IsAdaptDpiOn() const
{
	return isAdaptDpiOn_;
}

bool ConfigManager::IsModifyWindowOn() const
{
	return isManualAdaptDpiOn_;
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

int32_t ConfigManager::GetFolderDialogType() const
{
	return folderDialogType_;
}

std::pair<int, std::wstring> ConfigManager::GetGuiStyleInfo() const
{
	return PkpmConfigManager::GetGuiStyleInfo();
}

//ǰ��û��˵��Ϊʲô��Ҫ������������ʶ����
void ConfigManager::SetGuiStyleInfo(const std::pair<int, std::wstring>& info)
{
	return PkpmConfigManager::SetGuiStyleInfo(info);
}