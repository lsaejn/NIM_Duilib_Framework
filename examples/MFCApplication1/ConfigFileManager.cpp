#include "pch.h"
#include "ConfigFileManager.h"
#include "nlohmann/json.hpp"
#include <fstream>

/*
enableAdaptDpi为真，则使用duilib的适放机制。需要修改资源以避免图片模糊。
enableAdaptDpi为假，则:
1.enableModifyScaleForCef为真 ,cef随屏幕变化，标题栏拉宽。
2.cef和标题栏都默认。
因该用类来控制，但...公司并没有给我分配工期
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
		if (json.find(u8"enableAutoModifyWindow") != json.end())
		{
			isAutoModifyWindowOn_ = json[u8"enableAutoModifyWindow"];
		}
		if (json.find(u8"enableAdaptDpi") != json.end())
		{
			isAdaptDpiOn_ = json[u8"enableAdaptDpi"];
		}
		if (json.find(u8"systemFolderSelection") != json.end())
		{
			systemFolderSelection_ = json[u8"systemFolderSelection"];
		}
		//老代码是ansi,这里先不动了
		if (json.find("defaultAdvertise") != json.end())
		{
			defaultAdvertise_=json[u8"defaultAdvertise"].dump();
		}
		if (json.find("server") != json.end())
		{
			advertisementServer_ =nbase::UTF8ToUTF16( json["server"]);
		}
		if (json.find("query") != json.end())
		{
			advertisementQuery_ = nbase::UTF8ToUTF16( json["query"]);
		}
		if (json.find("windowText") != json.end())
		{
			cefFormWindowText_ = nbase::UTF8ToUTF16( json["windowText"]);
		}
		if (json.find("className") != json.end())
		{
			cefFormClassName_ = nbase::UTF8ToUTF16( json["className"]);
		}
		relativePathForHtmlRes_= nbase::UTF8ToUTF16(json["relativePathForHtmlRes"]);
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