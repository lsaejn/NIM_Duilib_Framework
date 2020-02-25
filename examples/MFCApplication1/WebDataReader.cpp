#include "pch.h"
#include "WebDataReader.h"
#include "xml2json.h"

#include <filesystem>


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
        //if (std::wstring_view(filePath).ends_with(L".xml")); continue;
		std::string content;
		bool succ = nbase::ReadFileToString(filePath, content);
		ASSERT(succ);
        std::string u8fileName = p.path().filename().generic_u8string();
        u8fileName = kRelativePathForPkpmAppMenuAnsi + u8fileName;
        xmlData_[u8fileName] = xml2json(nbase::AnsiToUtf8(content).c_str());
        //std::string ansi_json = xml2json(xmlStr.c_str());
    }

}