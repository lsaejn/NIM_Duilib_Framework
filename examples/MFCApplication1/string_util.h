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