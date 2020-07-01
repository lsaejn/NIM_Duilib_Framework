#pragma once
#include <map>
#include <string>
#include <optional>
#include <iostream>

#include<windows.h>
#include "ConfigFileCheck.h"



class App
{
	 const char* defaultConfigFile_ = "PkpmInfo.ini";

	std::optional<std::string> GetConfigFile(int argc, char** argv)
	{
		if (2 == argc)
		{
			if (!fs::FilePathIsExist(argv[1], false))
			{
				std::cout << "�����д����˲����ڵ��ļ�" << std::endl;
				std::exit(-1);
			}
			else
				return { argv[1] };
		}
		std::string appPath = fs::GetAppPath();
		std::string configFilePath = appPath + defaultConfigFile_;
		if (fs::FilePathIsExist(configFilePath, false))
		{	
			return configFilePath;
		}
		std::cout << "��ǰ·����û���ҵ������ļ�, ��ע�������" << std::endl;
		return {};
	}

public:

	int Run(int argc, char** argv)
	{
		auto op = GetConfigFile(argc, argv);
		if (op.has_value())
		{
			Launcher l(op.value());
			l.OpenPKPMWithConfigFile();
		}
		else
			Launcher().OpenPKPMNoConfigFile();
#ifdef _DEBUG
		getchar();
#endif // DEBUG

		return 0;
	}
};