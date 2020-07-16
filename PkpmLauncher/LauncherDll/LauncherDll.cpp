// LauncherDll.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <algorithm>
#include <string_view>
#include <unordered_map>

#include "FileSystem.h"
#include "tinyxml2.h"

class DeleterFileClose {
public:
    inline void operator()(FILE* x) const {
        fclose(x);
    }
};

int64_t GetFileSize(const std::wstring& filepath)
{
    WIN32_FIND_DATAW file_data;
    HANDLE file = FindFirstFileW(filepath.c_str(), &file_data);

    if (file == INVALID_HANDLE_VALUE)
        return -1;
    LARGE_INTEGER li = { file_data.nFileSizeLow, (LONG)file_data.nFileSizeHigh };
    FindClose(file);
    return li.QuadPart;
}

FILE* OpenFile(const wchar_t* filepath, const wchar_t* mode)
{
    return _wfsopen(filepath, mode, _SH_DENYNO);
}

bool ReadFileToString(const std::wstring& filepath, std::string& out)
{
    std::unique_ptr<FILE, DeleterFileClose> file;
    file.reset(OpenFile(filepath.c_str(), L"rb"));

    if (!file)
        return false;

    size_t file_size = (size_t)GetFileSize(filepath);
    if (file_size > size_t(-1)) //x64编译条件下，size_t的最大值转为int64_t会溢出，因此用无符号
        return false; // Too large

    bool read_ok = true;
    out.resize(file_size);
    if (!out.empty()) {
        read_ok = fread(&out[0], 1, out.size(), file.get()) == out.size();
    }

    return read_ok;
}

std::string UTF8ToGBK(const std::string& strUTF8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)strUTF8.c_str(), -1, wszGBK, len);

    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    //strUTF8 = szGBK;
    std::string strTemp(szGBK);
    delete[]szGBK;
    delete[]wszGBK;
    return strTemp;
}

bool UnicodeToMBCS(const std::wstring& input, std::string& output, int code_page)
{
    output.clear();
    int length = ::WideCharToMultiByte(code_page, 0, input.c_str(), static_cast<int>(input.size()), NULL, 0, NULL, NULL);
    output.resize(length);
    if (output.empty())
        return true;
    ::WideCharToMultiByte(code_page,
        0,
        input.c_str(),
        static_cast<int>(input.size()),
        &output[0],
        static_cast<int>(output.size()),
        NULL,
        NULL);
    return true;
}

std::string GetExePath()
{
    char buffer[1024*16];
    GetModuleFileNameA(NULL, buffer, sizeof(buffer) / sizeof(*buffer));
    int pos = -1;
    int index = 0;
    while (buffer[index])
    {
        if (buffer[index] == L'\\' || buffer[index] == L'/')
        {
            pos = index;
        }
        index++;
    }
    return std::string(buffer, pos + 1);
}

#include "json.hpp"
class LauncherDll
{
public:
    std::string filename_;
    LauncherDll()
    {
        Alime::FileSystem::File file(L".\\resources\\HtmlRes\\PKPMAPPV51.json");
        UnicodeToMBCS(file.GetFilePath().GetFullPath(), filename_, CP_ACP);
        std::string content;
        ReadFileToString(file.GetFilePath().GetFullPath(), content);
        content = content.substr(content.find('{'));
        nlohmann::json json = nlohmann::json::parse(content);
        std::cout << UTF8ToGBK(content) << std::endl;
        auto jsonInTop = json["PKPMMENU"]["TOP"];
        for (auto& elem : jsonInTop)
        {
            std::string xml = elem["XMLFILE"];
            std::cout << UTF8ToGBK(xml) << std::endl;
        }
    }

    bool Init()
    {
        Alime::FileSystem::Folder folder(L"./CFG/PKPMAPPMENU");
        std::vector<Alime::FileSystem::File> files;
        folder.GetFiles(files);
        auto iter = std::remove_if(files.begin(), files.end(), [](Alime::FileSystem::File& file) {
            auto str = file.GetFilePath().GetName();
            if (str.length() <= 4)
                return false;
            std::wstring suffix = str.substr(str.length() - 4);
            std::wstring_view sv = suffix;
            std::wstring_view sv2 = str;
            return (!sv._Equal(L".xml") || !sv._Equal(L".XML"))
                && !sv2._Starts_with(L"PKPMAPP")
                //|| str.find(L"UPDATE")!=std::string::npos
                //|| str.find(L"HELP") != std::string::npos
                ;
            });
        files.erase(iter, files.end());
        for (auto& file : files)
        {
            auto str = file.GetFilePath().GetFullPath();
            Init(str.c_str());

        }
        return 1;
    }

    bool Init(const wchar_t* filename)
    {
        tinyxml2::XMLDocument doc;
        std::string filenameA;
        UnicodeToMBCS(filename, filenameA, CP_ACP);
        doc.LoadFile(filenameA.c_str());
        auto f = doc.FirstChildElement("TOP");
        auto panels=doc.FirstChildElement("TOP")->FirstChildElement("CATEGORY")->FirstChildElement("PANELS");
        auto panel = panels->FirstChildElement("PANEL");
        while (panel)
        {
            std::string panelName= panel->FirstChildElement("NAME")->GetText();
            auto ret = panelName == "CPM";
            auto item = panel->FirstChildElement("ITEMS")->FirstChildElement("ITEM");
            while (item)
            {
                auto key= panelName + "|";
                std::string path=item->FirstChildElement("PATH")->GetText();
                if (path == "RIBBON-RELEASE")
                {
                    path = "RIBBON";
                }
                path = GetExePath() + path+"\\";
                std::string exeStr=item->FirstChildElement("RUNEXE")->GetText();
                path += exeStr;
                key+=item->FirstChildElement("NAME")->GetText();
                col.insert({key, path});
                item = item->NextSiblingElement();
            }
            panel = panel->NextSiblingElement();
        }
        return 1;
    }

    bool Run()
    {
        return 1;
    }

    std::unordered_map<std::string, std::string> col;
};

int main()
{
    LauncherDll dll;
    dll.Init();
    std::cout << "Hello World!\n";
}
