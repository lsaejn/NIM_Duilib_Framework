#include "pch.h"
#include "ConfigFileManager.h"
#include "nlohmann/json.hpp"
#include <fstream>

/*
enableAdaptDpi为真，则使用duilib的适放机制。需要修改资源以避免图片模糊。
enableAdaptDpi为假，则完全自己处理
*/
void ConfigManager::CheckAdaptDpi()
{
	{
		std::ifstream ifs(nbase::UnicodeToAnsi(filePath_));
		if (!ifs.good())
			return;
		nlohmann::json json;
		ifs >> json;
		isAutoModifyWindowOn_ = json[u8"enableAutoModifyWindow"];
		isAdaptDpiOn_ = json[u8"enableAdaptDpi"];
	}
}

ConfigManager::ConfigManager()
	:isAutoModifyWindowOn_(false),
	isAdaptDpiOn_(false),
	deadline_(INT32_MAX),
	filePath_(nbase::win32::GetCurrentModuleDirectory()
		+ L"resources\\themes\\default\\defaultConfig.json")
{
	LoadConfigFile();
}


void ConfigManager::LoadConfigFile()
{
	std::string content;
	bool succ = nbase::ReadFileToString(filePath_, content);
	if (succ)
	{
		nlohmann::json json;
		json = nlohmann::json::parse(content);
		try {
			isAutoModifyWindowOn_ = json[u8"enableAutoModifyWindow"];
			isAdaptDpiOn_ = json[u8"enableAdaptDpi"];
			systemFolderSelection_ = json[u8"systemFolderSelection"];
			defaultAdvertise_ = json[u8"defaultAdvertise"].dump();
			advertisementServer_ = nbase::UTF8ToUTF16(json["server"]);
			advertisementQuery_ = nbase::UTF8ToUTF16(json["query"]);
			cefFormWindowText_ = nbase::UTF8ToUTF16(json["windowText"]);
			cefFormClassName_ = nbase::UTF8ToUTF16(json["className"]);
			relativePathForHtmlRes_ = nbase::UTF8ToUTF16(json["relativePathForHtmlRes"]);
			deadline_ = json["deadline"];
			if (deadline_ <= 0)
				deadline_ = 7;
		}
		catch (...)
		{
			::AfxMessageBox(L"读取配置文件失败");
			std::abort();
		}
	}
}

bool ConfigManager::IsSystemFolderDialogOn() const
{
	return systemFolderSelection_;
}

std::string ConfigManager::GetDefaultAdvertise() const
{
	return defaultAdvertise_;
}

std::wstring ConfigManager::GetAdvertisementServer() const
{
	return advertisementServer_;
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

bool ConfigManager::IsAdaptDpiOn()
{
	return isAdaptDpiOn_;
}

bool ConfigManager::IsModifyWindowOn()
{
	return isAutoModifyWindowOn_;
}

int32_t ConfigManager::DaysLeftToNotify() const
{
	return deadline_;
}