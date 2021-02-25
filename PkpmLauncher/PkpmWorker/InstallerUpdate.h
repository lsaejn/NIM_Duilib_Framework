#pragma once

#include <string>
#include "ThirdParty/md5/md5.h"
#include "base/base.h"
#include "ConfigFileManager.h"
#include "Alime/HttpUtil.h"
#include "Task.h"
#include "ThirdParty/nlohmann/json.hpp"

/// <summary>
/// 后台更新installer.exe
/// 但是客户需要自己建内网服务器，为了让用户只改ip即可工作
/// 我放弃这个方案
/// </summary>


class InstallerUpdater: public QueryTask
{
public:
	InstallerUpdater()
	{

	}

	~InstallerUpdater()
	{

	}

	void Run()
	{

	}

private:
	std::string fileContent_;

	bool IsLatestVersion()
	{
		return true;
	}

	bool GetInstallerJsonFile()
	{
		Alime::HttpUtility::HttpRequest req;
		req.server = ConfigManager::GetInstance().GetAdvertisementServer();
		req.query = ConfigManager::GetInstance().GetInstallerPath();
		req.acceptTypes.push_back(L"text/html");
		req.acceptTypes.push_back(L"application/xhtml+xml");
		req.method = L"Get";
		req.port = 80;
		req.secure = false;
		Alime::HttpUtility::HttpResponse res;
		HttpQuery(req, res);
		if (200 != res.statusCode)
			return false;
		//we can not check whether the file's valid
		fileContent_ = nbase::UnicodeToAnsi(res.GetBodyUtf8());
		if (!fileContent_.empty())
		{
			try
			{
				nlohmann::json json = nlohmann::json::parse(fileContent_);
				//json[""]
			}
			catch (const std::exception& e)
			{

			}
		}
	}

	std::string InstallerPath()
	{
		auto installerPath = nbase::win32::GetCurrentModuleDirectory() + L"..\\installer.exe";
		return {};
	}

	std::string GetSum()
	{
		//md5file()
		return {};
	}
};
