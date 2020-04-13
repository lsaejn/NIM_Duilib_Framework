#include "pch.h"
#include "ConfigFileManager.h"
#include "nlohmann/json.hpp"
#include <fstream>

/*
enableAdaptDpiΪ�棬��ʹ��duilib���ʷŻ��ơ���Ҫ�޸���Դ�Ա���ͼƬģ����
enableAdaptDpiΪ�٣���:
1.enableModifyScaleForCefΪ�� ,cef����Ļ�仯������������
2.cef�ͱ�������Ĭ�ϡ�
������������ƣ���...��˾��û�и��ҷ��乤��
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
		//�ϴ�����ansi,�����Ȳ�����
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