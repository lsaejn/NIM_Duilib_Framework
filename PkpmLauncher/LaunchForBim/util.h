#pragma once
#include <string>
#include <algorithm>
#include <vector>

#include "windows.h"
#include <io.h>

namespace util
{
	static bool startWith(const char* src, const char* des)
	{
		return (strlen(des) <= strlen(src)) && (!memcmp(src, des, strlen(des)));
	}

	static bool endWith(const char* src, const char* des)
	{
		int offset = strlen(src) - strlen(des);
		return  offset >= 0
			&& startWith(src + offset, des);
	}

	// string alway begin with "", but not support here~~
	static bool startWith(const char* src, const char* des, bool sensitive)
	{
		if (sensitive)
		{
			return startWith(src, des);
		}
		else
		{
			if (strlen(des) > strlen(src))
				return false;
			for (size_t i = 0; i != strlen(des); ++i)
			{
				if (std::toupper(src[i]) != std::toupper(des[i]))
					return false;
			}
		}
		return true;
	}

	static bool endWith(const char* src, const char* des, bool sensitive)
	{
		if (sensitive)
		{
			return endWith(src, des);
		}
		else
		{
			int offset = strlen(src) - strlen(des);
			return  offset >= 0
				&& startWith(src + offset, des, false);
		}
	}

	static bool is_space(const char c)
	{
		return std::isspace(c) != 0; 
	}

	static std::string trim(std::string&& s)
	{
		s.erase(std::find_if_not(s.rbegin(), s.rend(), is_space).base(), s.end());
		s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), is_space));
		return std::move(s);
	}

	/// <summary>
	/// note that suffix is a common file-end with '.'
	/// </summary>
	/// <param name="path"></param>
	/// <param name="prefix"></param>
	/// <param name="suffix"></param>
	/// <returns></returns>
	static std::vector<std::string> FindFiles(const char* path, const char* prefix, const char* suffix)
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
			if (startWith(fileinfo.name, prefix, false) && endWith(fileinfo.name, suffix, false))
			{
				std::string filename(fileinfo.name);
				result.push_back(filename);
			}
		} while (!_findnext(handle, &fileinfo));
		_findclose(handle);
		return result;
	}
}
