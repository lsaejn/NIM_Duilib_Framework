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

//���ڳ�������ע���������exeΪ��
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
        ::AfxMessageBox(L"���İ�װ���ܲ�����,�޷��ҵ�CFG/PKPMAPPMENU�ļ���");
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
            MessageBox(NULL, filePath.c_str(), L"��ȡ�����ļ�����", MB_ICONINFORMATION);
        //������Ҫ�ģ���Ӧ�ô�����·��,�����һ���u8�ġ�
        //������Ҫ���������������ǰ�˴��ݵ��ļ������˼���·����
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
            MessageBox(NULL, filePath.c_str(), L"��Ч�������ļ�", MB_ICONINFORMATION);
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