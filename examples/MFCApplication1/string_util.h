#pragma once
#include <vector>
#include <string>


template<typename Ty, int nSize>
int ArraySize(Ty(&Arg)[nSize])
{
	return nSize;
}
namespace string_utility
{
	bool startWith(const char* src, const char* des);
	bool endWith(const char* src, const char* des);
	std::vector<std::string> string_split(const std::string& s, std::string&& c);
}

//基本上是为了替换掉老代码的pkpmsvr.h/cpp和 pkpm2014svr.h/cpp
namespace application_utily
{	
	std::wstring FullPathOfPkpmIni();
	std::string FullPathOfPkpmIniA();
	std::string DictToJson(const std::vector<std::pair<std::string, std::string>>& dict);
	std::string FileEncode(const std::string& query);
	void OpenDocument(const std::wstring& filePath);
}