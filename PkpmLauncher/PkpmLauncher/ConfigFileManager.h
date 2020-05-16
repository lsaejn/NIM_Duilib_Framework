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

	bool IsAdaptDpiOn();
	bool IsModifyWindowOn();
	bool IsSystemFolderDialogOn() const;
	std::string GetDefaultAdvertise() const;
	std::wstring GetAdvertisementServer() const;
	std::wstring GetAdvertisementQuery() const;
	std::wstring GetCefFormWindowText() const;
	std::wstring GetCefFormClassName() const;
	std::wstring GetRelativePathForHtmlRes() const;
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
	std::string defaultAdvertise_;
	std::wstring advertisementServer_;
	std::wstring advertisementQuery_;
	std::wstring cefFormWindowText_;
	std::wstring cefFormClassName_;
	std::wstring relativePathForHtmlRes_;
	std::wstring filePath_;
	int32_t styleNo_;//界面风格编号
	int32_t deadline_;
	nlohmann::json json_;
};
