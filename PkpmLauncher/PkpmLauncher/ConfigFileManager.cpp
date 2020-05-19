#include "pch.h"
#include "ConfigFileManager.h"
#include <fstream>
#include "Alime/FileSystem.h"

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
	styleNo_(0),
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
		 json_ = nlohmann::json::parse(content);
		auto& json = json_;
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
			skinFile_ = nbase::UTF8ToUTF16(json["skinFile"]);
			skinFolder_ = nbase::UTF8ToUTF16(json["skinFolder"]);
			if(json.find("interfaceStyleNo")!=json.end())//我们不知道什么时候上线这个功能
				styleNo_ = json["interfaceStyleNo"];
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

std::wstring ConfigManager::GetSkinFilePath() const
{
	return skinFile_;
}
std::wstring ConfigManager::GetSkinFolderPath() const
{
	return skinFolder_;
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

int32_t ConfigManager::GetInterfaceStyleNo() const
{
	return styleNo_;
}

//立刻保存是因为启动器可能多开
void ConfigManager::SetInterfaceStyleNo(int32_t no)
{
	styleNo_ = no;
	if (no != json_["interfaceStyleNo"])
	{
		json_["interfaceStyleNo"] = no;		
		std::fstream file;
		auto newFile = filePath_ + L"copy";
		file.open(newFile, std::ios_base::out | std::ios_base::trunc);
		if (file.is_open()&& file.good())
		{		
			auto str=json_.dump();
			file.write(str.c_str(), str.size());
		}
		file.close();

		Alime::FileSystem::File originFile = filePath_;
		if (!originFile.Rename(L"backup"))
			return;
		auto fileName = Alime::FileSystem::FilePath(filePath_).GetName();
		auto filePath = Alime::FileSystem::FilePath(filePath_).GetFolder().GetFullPathWithSurfix();
		Alime::FileSystem::File fileToSave = newFile;			
		if (!fileToSave.Rename(fileName))
		{
			originFile.Rename(fileName);
			fileToSave.Delete();
		}
		else
		{
			Alime::FileSystem::File(filePath + L"backup").Delete();
		}
	}	
	
}