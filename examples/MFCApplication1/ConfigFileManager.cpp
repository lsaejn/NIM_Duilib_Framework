#include "pch.h"
#include "ConfigFileManager.h"
#include "nlohmann/json.hpp"

/*
enableAdaptDpiΪ�棬��ʹ��duilib���ʷŻ��ơ���Ҫ�޸���Դ�Ա���ͼƬģ����
enableAdaptDpiΪ�٣���:
1.enableModifyScaleForCefΪ�� ,cef����Ļ�仯������������
2.cef�ͱ�������Ĭ�ϡ�
������������ƣ���...��˾��û�и��ҷ��乤��
*/
void ConfigManager::CheckAdaptDpi()
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
			isAdaptDpiOn_= json[u8"enableAdaptDpi"];
		}
	}	
}