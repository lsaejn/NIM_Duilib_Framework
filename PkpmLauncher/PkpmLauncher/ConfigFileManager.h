#pragma once
#include "Alime/NonCopyable.h"
#include "nlohmann/json.hpp"

//reader
class ConfigManager: public noncopyable
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
	std::wstring GetSkinFilePath() const;
	std::wstring GetSkinFolderPath() const;
	int32_t GetInterfaceStyleNo() const;
	void SetInterfaceStyleNo(int32_t);
	int32_t DaysLeftToNotify() const;
private:	
	ConfigManager();

	[[deprecated("why two dpi variable in this function?")]] 
	void CheckAdaptDpi();
	void LoadConfigFile();
private:
	bool isAutoModifyWindowOn_;
	bool isAdaptDpiOn_;
	bool systemFolderSelection_;
	bool isWebPageRefreshOn_;
	std::string defaultAdvertise_;
	std::wstring advertisementServer_;
	std::wstring advertisementQuery_;
	std::wstring cefFormWindowText_;
	std::wstring cefFormClassName_;
	std::wstring relativePathForHtmlRes_;
	std::wstring filePath_;
	std::wstring skinFile_;
	std::wstring skinFolder_;
	int32_t styleNo_;//��������
	int32_t deadline_;
	nlohmann::json json_;
};
