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
	styleIndex_(0),
	folderDialogType_(0),
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
		try
		{
			isAutoModifyWindowOn_ = json[u8"enableAutoModifyWindow"];
			isAdaptDpiOn_ = json[u8"enableAdaptDpi"];
			isWebPageRefreshOn_ = json[u8"enableRefresh"];
			systemFolderSelection_ = json[u8"systemFolderSelection"];
			defaultAdvertise_ = json[u8"defaultAdvertise"].dump();
			folderDialogType_ = json["folderDialogType"];
			advertisementServer_ = nbase::UTF8ToUTF16(json["server"]);
			advertisementQuery_ = nbase::UTF8ToUTF16(json["query"]);
			cefFormWindowText_ = nbase::UTF8ToUTF16(json["windowText"]);
			cefFormClassName_ = nbase::UTF8ToUTF16(json["className"]);
			relativePathForHtmlRes_ = nbase::UTF8ToUTF16(json["relativePathForHtmlRes"]);
			skinFile_ = nbase::UTF8ToUTF16(json["skinFile"]);
			skinFolder_ = nbase::UTF8ToUTF16(json["skinFolder"]);
			nativeArticlesPath_ = nbase::UTF8ToUTF16(json["nativeArticles"]);
			webArticlesPath_ = nbase::UTF8ToUTF16(json["webArticles"]);
			styleIndex_ = json["guiStyleInfo"]["styleIndex"];
			styleName_ = nbase::UTF8ToUTF16(json["guiStyleInfo"]["styleName"]);
			deadline_ = json["deadline"];
			deadline_ = deadline_ <= 0 ? 7 : deadline_;
		}
		catch (...)
		{
			::AfxMessageBox(L"读取配置文件 defaultConfig.json 失败");
			std::abort();
		}
	}
	else
	{
		::AfxMessageBox(L"无法找到 defaultConfig.json 失败");
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

std::wstring ConfigManager::GetNativeArticlePath() const
{
	return nativeArticlesPath_;
}

std::wstring ConfigManager::GetWebArticlePath() const
{
	return webArticlesPath_;
}

bool ConfigManager::IsAdaptDpiOn() const
{
	return isAdaptDpiOn_;
}

bool ConfigManager::IsModifyWindowOn() const
{
	return isAutoModifyWindowOn_;
}

int32_t ConfigManager::DaysLeftToNotify() const
{
	return deadline_;
}

int32_t ConfigManager::GetFolderDialogType() const
{
	return folderDialogType_;
}

std::pair<int, std::wstring> ConfigManager::GetGuiStyleInfo() const
{
	return {styleIndex_, styleName_};
}

//前端没有说明为什么需要两个变量来标识主题
//windows不用u8的原因是....?
void ConfigManager::SetGuiStyleInfo(const std::pair<int, std::wstring>& info)
{
	styleIndex_ = info.first;
	styleName_ = info.second;
	if (styleIndex_ != json_["guiStyleInfo"]["styleIndex"] ||
		styleName_!=  json_["guiStyleInfo"]["styleName"]
		)
	{
		json_["guiStyleInfo"]["styleIndex"] = styleIndex_;
		json_["guiStyleInfo"]["styleName"] = nbase::UTF16ToUTF8(styleName_);

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