#pragma once
#include "FileSystem.h"
#include "util.h"
#include "ConfigFileCheck.h"

#include <assert.h>

/// <summary>
/// no unit_test because of lots of code are modified from alime
/// </summary>
class SimpleTest
{
public:
	void RunAll()
	{
		Check_Util();
		Check_FileSystem();
		Check_Launcher();
	}
	void Check_Util()
	{
		{
			assert(util::startWith("helloworld", "hello"));
			assert(util::startWith("helloworld", "helloworld"));
			assert(!util::startWith("helloworld", "ello"));
			assert(util::startWith("helloworld", ""));
			assert(util::startWith("", ""));
			assert(!util::startWith("", "aa"));
			assert(!util::startWith("a", "aa"));
		}
		{
			assert(util::endWith("helloworld", "world"));
			assert(util::endWith("helloworld", ""));
			assert(util::endWith("", ""));
			assert(!util::endWith("", "aaa"));
			assert(util::endWith("1", ""));
			assert(!util::endWith("1", "12"));
			assert(!util::endWith("1", "121"));
		}
		{
			assert(util::startWith("helloworld", "Hello", false));
			assert(util::startWith("helloworld", "", false));
			assert(util::startWith("heLLOworld", "Hello", false));
			assert(util::startWith("Helloworld", "hElLo", false));
			assert(util::startWith("", "", false));
			assert(!util::startWith("", "111", false));
		}
		{
			assert(util::endWith("helloworld", "World", false));
			assert(util::endWith("helloWWorld", "", false));
			assert(util::endWith("", "", false));
			assert(!util::endWith("", "aAA", false));
			assert(util::endWith("1", "", false));
			assert(util::endWith("1a", "", false));
			assert(!util::endWith("1", "12", false));
			assert(!util::endWith("1", "121", false));
			assert(util::endWith("1", "1", false));
			assert(util::endWith("1a", "a", false));
		}
		{
			auto vec=util::FindFiles("./", "", "h");
			assert(vec.size()==5);
			vec= util::FindFiles("./", "", "cpp");
			assert(vec.size() == 1);
			char buffer[MAX_PATH] = { 0 };
			GetCurrentDirectory(MAX_PATH, buffer);
			vec = util::FindFiles(buffer, "", "");
			assert(vec.size() >=2);
			vec = util::FindFiles("./", "", "cc");
			assert(vec.empty());
		}
	}

	void Check_FileSystem()
	{
		HANDLE h;
		fs::CreateProcessWithCommand("C://windows//system32//notepad.exe", "", &h);
		Sleep(2000);
		TerminateProcess(h, -1);
	}

	void Check_Launcher()
	{
		//
		{
			Regedit reg;
			std::vector<std::string> toFind = { "CFGV43", "CFGV51", "CFGV52", "CFGV53" };
			std::vector<std::string> found;
			std::vector<std::string> pathVec;
			for (size_t i = 0; i != toFind.size(); ++i)
			{
				auto str=reg.ReadSpecificKey(toFind[i]);
				if (!str.empty())
				{
					found.push_back(toFind[i]);
					pathVec.push_back(str);
				}
			}
			auto latest=reg.ReadLatestKey();
			if(!latest.empty())
				assert(std::find(pathVec.begin(), pathVec.end(), latest) != pathVec.end());
		}
	}
};

#ifdef SIMPLEDEBUG
SimpleTest().RunAll();
#endif // _DEBUG