#include "pch.h"
#include "WebDataReader.h"
#include "xml2json.h"
#include "nlohmann/json.hpp"

#include <filesystem>

//由于程序依赖注册表，所以以exe为根
const std::wstring kRelativePathForPkpmAppMenu = L"CFG\\PKPMAPPMENU\\";
const std::string kRelativePathForPkpmAppMenuAnsi = "CFG\\PKPMAPPMENU\\";

void WebDataReader::init()
{
	load();
}

const std::string WebDataReader::readAll() const
{
	return{};
}

const std::string WebDataReader::readSpecific(const std::string& filePath) const
{
    auto iter = xmlData_.find(filePath);
    if (iter != xmlData_.end())
        return iter->second;
    else
        return {};
}

void WebDataReader::load()
{
	//for()
    auto path = nbase::win32::GetCurrentModuleDirectory() + kRelativePathForPkpmAppMenu;
    if (!std::filesystem::exists(path))
    {
        ::AfxMessageBox(L"您的安装可能不完整,无法找到CFG/PKPMAPPMENU文件夹");
        std::abort();
    }
    for (auto& p : std::filesystem::directory_iterator(path))
    {
		std::wstring filePath = p.path().generic_wstring();
        OutputDebugString(filePath.c_str());
		std::string content;
		bool succ = nbase::ReadFileToString(filePath, content);
		ASSERT(succ);
        //这里需要改，不应该存完整路径,，而且还是u8的。
        //不改主要是懒，其次我忘了前端传递的文件名带了几层路径。
        std::string u8fileName = p.path().filename().generic_u8string();
        u8fileName = kRelativePathForPkpmAppMenuAnsi + u8fileName;
        //
        std::string u8Content = xml2json(nbase::AnsiToUtf8(content).c_str());
        std::string modifiedU8;
        if (modifyDataWithTag(u8Content, modifiedU8))
        {
            xmlData_[u8fileName] = modifiedU8;
        }
        else
        {
            xmlData_[u8fileName] = u8Content;
        }          
    }
}

bool WebDataReader::modifyDataWithTag(const std::string& u8Content, std::string& result)
{
    nlohmann::json json = nlohmann::json::parse(u8Content);
    auto check = json["TOP"].find("CHECK");
    if (check != json["TOP"].end())
    {
        auto& jsonOfPanels = json["TOP"]["CATEGORY"]["PANELS"]["PANEL"];
#ifdef DEBUG
        auto debug_ = jsonOfPanels.dump();
        auto u16 = nbase::UTF8ToUTF16(debug_);
#endif // DEBUG
        for (int i = 0; i != jsonOfPanels.size(); ++i)
        {
            if (jsonOfPanels[i].find("SHOW") != jsonOfPanels[i].end())
            {
                if (jsonOfPanels[i]["SHOW"] == "false")
                    jsonOfPanels.erase(i);
            }
        }
        result = json.dump();
        return true;
    }
    return false;
}