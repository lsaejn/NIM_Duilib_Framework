#pragma once
#include <string>
#include <vector>
#include "string_util.h"
#include <io.h>

class FindSpecificFiles
{
public:
	static std::vector<std::string> FindFiles(const char* path,
		const char* prefix, const char* suffix)
	{
		std::string toFind(path);
		toFind += "\\*.";
		toFind += suffix;
		std::vector<std::string> result;
		long handle;
		_finddata_t fileinfo;
		handle = _findfirst(toFind.c_str(), &fileinfo);
		if (handle == -1)
			return result;
		do
		{
			if (string_utility::startWith(fileinfo.name, prefix)/* && endWith(fileinfo.name, ".ini")*/)
			{
				std::string filename(fileinfo.name);
				filename = filename.substr(1, filename.size() - 5);
				if (IsValidVersionName(filename))
					result.push_back(filename);
			}
		} while (!_findnext(handle, &fileinfo));
		_findclose(handle);
		return result;
	}
private:
	static bool IsValidVersionName(const std::string& str)
	{
		if (!isdigit(str.front()))
			return false;
		for (auto elem : str)
		{
			if (!isdigit(elem) && elem != '.')
				return false;
		}
		return true;
	}
};

class IVersionCompareStrategy
{
public:
	//返回a<b
	virtual bool operator()(const std::string& a, const std::string& b) = 0;
};

class AscendingOrder: public IVersionCompareStrategy
{
public:
	virtual bool operator()(const std::string& lhs, const std::string& rhs) override
	{
		auto lhsElems = string_utility::string_split(lhs, ".");
		auto rhsElems = string_utility::string_split(rhs, ".");
		size_t countOfLhs = lhsElems.size();
		size_t countOfRhs = rhsElems.size();
		size_t countToCompared = countOfLhs < countOfRhs ? countOfLhs : countOfRhs;
		for (size_t i = 0; i != countToCompared; ++i)
		{
			auto num1 = std::stoi(lhsElems[i]);
			auto num2 = std::stoi(rhsElems[i]);
			if (num1 != num2)
				return num1 < num2;
		}
		if (countOfLhs > countOfRhs)
			return false;
		for (size_t i = countToCompared; i != rhsElems.size(); ++i)
		{
			if (std::stoi(rhsElems[i]) > 0)//ok
				return true;
		}
		return false;
	}
};

class AlwaysLessWhenNotEuqual : public IVersionCompareStrategy
{
public:
	virtual bool operator()(const std::string& a, const std::string& b) override
	{
		if (a != b)
			return true;
	}
};

//怕是以后要带字母
class UnKnowStrategy : public IVersionCompareStrategy
{
public:
	virtual bool operator()(const std::string& a, const std::string& b) override
	{
		return a!=b;
	}
};




