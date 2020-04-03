#pragma once
#include "NoCopyable.h"

class ConfigManager: public noncopyable
{
public:
	static ConfigManager& GetInstance()
	{
		static ConfigManager instance;
		return instance;
	}

	bool IsAdaptDpiOn()
	{
		return isAdaptDpiOn_;
	}

	bool IsAutoModifyWindowOn()
	{
		return isAutoModifyWindowOn_;
	}

private:	
	ConfigManager()
		:isAutoModifyWindowOn_(false),
		isAdaptDpiOn_(false),
		filePath_(nbase::win32::GetCurrentModuleDirectory()
			+ L"resources\\themes\\default\\defaultConfig.json")
	{
		CheckAdaptDpi();
	}
	void CheckAdaptDpi();
private:
	bool isAdaptDpiForCaptionOn_;
	bool isAutoModifyWindowOn_;
	bool isAdaptDpiOn_;
	std::wstring filePath_;
};
