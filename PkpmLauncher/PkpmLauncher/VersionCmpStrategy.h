#pragma once
#include <string>
#include <vector>
#include "utility.h"
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
		int ret = Compare(lhs, rhs);
		if (ret == -1)
			return true;
		return false;
	}
private:
	//@return -1 when lhs<rhs , 0 when lhs==rhs, 1 when lhs>rhs
	int Compare(const std::string& lhs, const std::string& rhs)
	{
		auto lhsElems = StringsToIntegers(string_utility::string_split(lhs, "."));
		auto rhsElems = StringsToIntegers(string_utility::string_split(rhs, "."));
		TrimTailZeros(lhsElems); TrimTailZeros(rhsElems);
		auto numToCompare = lhsElems.size() <= rhsElems.size() ? lhsElems.size() : rhsElems.size();
		for (size_t i = 0; i != numToCompare; ++i)
		{
			if (lhsElems[i] != rhsElems[i])
				return lhsElems[i]<rhsElems[i]? -1: 1;
		}
		if (lhsElems.size()== rhsElems.size())
			return 0;
		return lhsElems.size() < rhsElems.size() ? -1 : 1;
	}

	std::vector<int> StringsToIntegers(const std::vector<std::string>& strs)
	{
		std::vector<int> numCol;
		for (size_t i = 0; i != strs.size(); ++i)
			numCol.push_back(std::stoi(strs[i]));
		return numCol;
	}

	void TrimTailZeros(std::vector<int>& nums)
	{
		size_t  i = nums.size()-1;
		for (; i >= 0; --i)
			if (nums[i] != 0)
				break;
		nums.resize(i + 1);
	}
};

class AlwaysLessWhenNotEuqual : public IVersionCompareStrategy
{
public:
	virtual bool operator()(const std::string& a, const std::string& b) override
	{
		if (a != b)
			return true;
		return false;
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




