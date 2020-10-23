#include "pch.h"
#include "WebDataReader.h"
#include "utility.h"
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
    {
        lastQueryFilePath_ = filePath;
        return iter->second;
    }     
    else
        return {};
}

std::string WebDataReader::LastQuery()
{
    return lastQueryFilePath_;
}

void WebDataReader::Load()
{
    auto path = nbase::win32::GetCurrentModuleDirectory() + kRelativePathForPkpmAppMenu;
    if (!std::filesystem::exists(path))
    {
        MsgBox::ShowViaID(L"ERROR_TIP_FIND_APPMENU_ERROR", L"TITLE_ERROR");
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
        if (!nbase::ReadFileToString(filePath, content)&& content.empty())
        {
            //没有结束程序是因为在一个部队机器上，出现读到了内容，但返回false, 猜测是文件大小被加密软件修改
            MsgBox::ShowViaIDWithSpecifiedCtn(filePath, L"TITLE_FIND_FILE_ERROR");
            continue;
        }
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
            MsgBox::ShowViaIDWithSpecifiedCtn(filePath, L"TITLE_CONFIG_ERROR");
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