#include "pch.h"
#include "WebDataReader.h"
#include "string_util.h"
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#include "xml2json.h"
#include "nlohmann/json.hpp"

#include <filesystem>
#include <algorithm>

//由于程序依赖注册表，所以以exe为根
const std::wstring kRelativePathForPkpmAppMenu = L"CFG\\PKPMAPPMENU\\";
const std::string kRelativePathForPkpmAppMenuAnsi = "CFG\\PKPMAPPMENU\\";

void WebDataReader::Init()
{
	Load();
}

const std::string WebDataReader::ReadAll() const
{
	return{};
}

const std::string WebDataReader::ReadSpecific(const std::string& filePath) const
{
    auto iter = xmlData_.find(filePath);
    if (iter != xmlData_.end())
        return iter->second;
    else
        return {};
}

void WebDataReader::Load()
{
    auto path = nbase::win32::GetCurrentModuleDirectory() + kRelativePathForPkpmAppMenu;
    if (!std::filesystem::exists(path))
    {
        ::AfxMessageBox(L"您的安装可能不完整,无法找到CFG/PKPMAPPMENU文件夹");
        std::abort();
    }
    for (auto& p : std::filesystem::directory_iterator(path))
    {
		std::wstring filePath = p.path().generic_wstring();
        {
            auto copy = nbase::UnicodeToAnsi(filePath);
            nbase::UpperString(copy);
            std::transform(copy.begin(), copy.end(), copy.begin(), [](char c) {
                if (c >= 'a' && c<= 'z')
                    c -= 'a' - 'A'; return c;
                });
            if (!string_utility::endWith(copy.c_str(), ".XML"))
                continue;
        }     
		std::string content;
		bool succ = nbase::ReadFileToString(filePath, content);
		if(!succ)
            MessageBox(NULL, filePath.c_str(), L"读取配置文件错误", MB_ICONINFORMATION);
        //这里需要改，不应该存完整路径,，而且还是u8的。
        //不改主要是懒，其次我忘了前端传递的文件名带了几层路径。
        std::string u8fileName = p.path().filename().generic_u8string();
        u8fileName = kRelativePathForPkpmAppMenuAnsi + u8fileName;
        //
        std::string u8Content;
        try 
        {
            u8Content = xml2json(nbase::AnsiToUtf8(content).c_str());
        }
        catch (...)
        {
            //log_err<<
            MessageBox(NULL, filePath.c_str(), L"无效的配置文件", MB_ICONINFORMATION);
            continue;
        }
        std::string modifiedU8;
        if (ModifyDataWithTag(u8Content, modifiedU8))
            xmlData_[u8fileName] = modifiedU8;
        else
            xmlData_[u8fileName] = u8Content;
    }
}

bool WebDataReader::ModifyDataWithTag(const std::string& u8Content, std::string& result)
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
        for (size_t i = 0; i != jsonOfPanels.size(); ++i)
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

void WebDataReader::Accept(IWebDataVisitor* visitor)
{
    visitor->Visit(this);
}