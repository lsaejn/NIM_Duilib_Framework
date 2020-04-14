#pragma once
#include "Alime/NonCopyable.h"

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

private:	
	ConfigManager();
	//·ÏÆú
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
};
