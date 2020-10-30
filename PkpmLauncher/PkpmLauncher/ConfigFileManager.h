#pragma once

#include "nlohmann/json.hpp"

#include "Alime/NonCopyable.h"


/*
expamle: 
ALIME_PROPERTY(std::string, defaultAdvertise_, DefaultAdvertise)
*/
#define ALIME_PROPERTY(varType, varName, prefix, funName) \
private: varType varName##_; \
public: varType prefix##funName() const; \

#define CLASS_MEMBER_WSTRING(varName, MainDescribe) \
ALIME_PROPERTY(std::wstring, varName, Get, MainDescribe)

#define CLASS_MEMBER_STRING(varName, MainDescribe) \
ALIME_PROPERTY(std::string, varName, Get, MainDescribe)

#define CLASS_MEMBER_BOOL(varName) \
private: bool is##varName##_; \
public: bool Is##varName() const{ return is##varName##_;}; \



//reader
class ConfigManager : public noncopyable
{
public:
	static ConfigManager& GetInstance()
	{
		static ConfigManager instance;
		return instance;
	}
	bool IsAdaptDpiOn() const;
	bool IsModifyWindowOn() const;
	bool IsSystemFolderDialogOn() const;
	bool IsWebPageRefreshOn() const;
	std::string GetDefaultAdvertise() const;
	std::wstring GetAdvertisementServer() const;
	std::wstring GetAdvertisementQuery() const;
	std::wstring GetCefFormWindowText() const;
	std::wstring GetCefFormClassName() const;
	std::wstring GetRelativePathForHtmlRes() const;
	std::wstring GetSkinFile() const;
	std::wstring GetSkinFolderPath() const;
	std::wstring GetNativeArticlePath() const;
	std::wstring GetWebArticlePath() const;
	std::wstring GetInstallerPath() const;
	std::pair<int, std::wstring> GetGuiStyleInfo() const;
	void SetGuiStyleInfo(const std::pair<int, std::wstring>& info);
	int32_t GetFolderDialogType() const;
	int32_t DaysLeftToNotify() const;
	std::wstring GetLaunchDllName() const;
	std::wstring GetBimDownLoadWeb() const;
private:	
	ConfigManager();

	[[deprecated("why two dpi variable in this function?")]] 
	void CheckAdaptDpi();
	void LoadConfigFile();

private:
	bool isManualAdaptDpiOn_;
	bool isAdaptDpiOn_;
	bool systemFolderSelection_;
	bool isWebPageRefreshOn_;
	std::string defaultAdvertise_;
	std::wstring launchDllName_;
	std::wstring server_;
	std::wstring advertisementQuery_;
	std::wstring cefFormWindowText_;
	std::wstring cefFormClassName_;
	std::wstring relativePathForHtmlRes_;
	std::wstring filePath_;
	std::wstring skinFile_;
	std::wstring skinFolder_;
	std::wstring nativeArticlesPath_;
	std::wstring webArticlesPath_;
	std::wstring styleName_;//只有前端用，应改为u8
	std::wstring bimWebUrl_;
	std::wstring installerPath_;
	int32_t styleIndex_;
	int32_t deadline_;
	int32_t folderDialogType_;
	nlohmann::json json_;
};
