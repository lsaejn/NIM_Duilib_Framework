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
			isAdaptDpiOn_= json[u8"enableAdaptDpi"];
		}
	}	
}