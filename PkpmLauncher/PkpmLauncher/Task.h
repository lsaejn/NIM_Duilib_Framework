#pragma once

#include "assert.h"

#include <string>
#include <regex>
#include <memory>

#include "RapidjsonForward.h"
#include "base/base.h"

#include "Lock_util.h"
#include "Alime/HttpUtil.h"
#include "Alime/NonCopyable.h"
#include "ConfigFileManager.h"

using namespace Alime::HttpUtility;
typedef std::function<void()> Functor;

class QueryTask: public noncopyable
{
public:
	virtual void Run(Functor after) = 0;
	virtual ~QueryTask() = default;
};

class FileDownLoader : public QueryTask
{
public:
	void Run(Functor after) override
	{
		assert(!data_.lock()->isWebPageCooked_);
		bool AdPageCanAccess = false;
		AdPageCanAccess = Download();
		data_.lock()->isWebPageCooked_ = AdPageCanAccess;
		if(after)
			after();
	}

	struct innerData
	{
		bool isWebPageCooked_ = false;
		std::string pageInfo_;
	};
	SafeAccess<innerData> data_;
	virtual bool Download() = 0;
};

class WebPageDownLoader: public FileDownLoader
{
private:
	bool Download()
	{
		HttpRequest req;
		req.server = ConfigManager::GetInstance().GetAdvertisementServer();
		req.query = ConfigManager::GetInstance().GetAdvertisementQuery();
		req.acceptTypes.push_back(L"text/html");
		req.acceptTypes.push_back(L"application/xhtml+xml");
		req.method = L"Get";
		req.port = 80;
		req.secure = false;
		HttpResponse res;
		HttpQuery(req, res);
		if (200 != res.statusCode)
			return false;
		auto result = res.GetBodyUtf8();
		auto astring = nbase::UnicodeToAnsi(result.c_str());
		std::regex reg("(<(body)>)([\\s\\S]*)(</\\2>)");
		if (_MSC_VER <= 1600)
			astring.erase(std::remove_if(astring.begin(), astring.end(), [](char c) {
			return c == '\n' || c == '\r' || c == ' '; }),
				astring.end());
		std::smatch match;
		if (std::regex_search(astring, match, reg))
		{
			std::string page = match[3];
			//考虑用户内网网页重定向的问题,简单处理即可
			if (page.find("UpdateUrl") == std::string::npos ||page.find("Version") == std::string::npos)
				return false;
			else
			{
				data_.lock()->pageInfo_ = page;
				return true;
			}			
		}
		return false;
	}
public:
	static std::string ParseWebPage(const std::string& pageInfo)
	{
		try
		{
			std::vector<std::pair<std::string, std::string>> data;
			rapidjson::Document document;
			document.Parse(pageInfo.c_str());
			auto& arr = document["Advertise"]["NationWide"];
			for (size_t i = 0; i < arr.Size(); ++i)
			{
				std::string adver_content(arr[i]["Advertisement"].GetString());
				std::string adver_url(arr[i]["Url"].GetString());
				data.push_back(std::make_pair(adver_content, adver_url));
			}
			rapidjson::Document doc;
			rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
			rapidjson::Value array(rapidjson::kArrayType);//< 创建一个数组对象
			for (size_t i = 0; i != data.size(); ++i)
			{
				rapidjson::Value obj(rapidjson::kObjectType);
				rapidjson::Value content(rapidjson::kStringType);
				content.SetString(data[i].first.c_str(), allocator);
				obj.AddMember("key", content, allocator);
				rapidjson::Value url(rapidjson::kStringType);
				url.SetString(data[i].second.c_str(), allocator);
				obj.AddMember("value", url, allocator);
				array.PushBack(obj, allocator);
			}
			rapidjson::Value root(rapidjson::kObjectType);
			root.AddMember("data", array, allocator);
			rapidjson::StringBuffer s;
			rapidjson::Writer<rapidjson::StringBuffer> writer(s);
			root.Accept(writer);
			return nbase::AnsiToUtf8(s.GetString());
		}
		catch (...)
		{
			return ConfigManager::GetInstance().GetDefaultAdvertise();
		}
	}
};

//no need to lock member
class AuthorizationCodeDate : public QueryTask, 
	public std::enable_shared_from_this<AuthorizationCodeDate>
{
public:
	void Run(Functor after) override
	{
		std::string sn;
		int daysLeft = RemainingTimeOfUserLock(&sn);
		if (daysLeft >= -1 && daysLeft < ConfigManager::GetInstance().DaysLeftToNotify())
		{
			data_.daysLeft_ = daysLeft;
			data_.authorizationCode_ = sn;
			if (after)
				after();
		}
	}

	struct innerData
	{
		int daysLeft_ = -1;
		std::string authorizationCode_;
	};
	innerData data_;
private:
	int RemainingTimeOfUserLock(std::string* SerialNumber)
	{
		//参数:@ 模块号, @子模块号 @返回的授权码
		//返回值: 剩余天数。网络锁返回-1。无法联网返回0。到达临界日期后，返回[0,剩余天数)
		//fix me。区分返回值0是不能上网还是剩余0天
		typedef int(_stdcall* FuncInWinAuthorize)(int&, int&, char*);
		auto pathOfWinAuthorize = nbase::win32::GetCurrentModuleDirectory() + L"Ribbon\\WinAuthorize.dll";
		auto handle = LoadLibrary(pathOfWinAuthorize.c_str());
		if (!handle)
			return -1;
		FuncInWinAuthorize dayLeftFunc = NULL;
		dayLeftFunc = (FuncInWinAuthorize)GetProcAddress(handle, "_Login_SubMod2@12");
		if (!dayLeftFunc)
		{
			FreeLibrary(handle);
			return -1;
		}
		int ty = 100;
		int sub_ky = 0;
		char gSN[17] = { 0 };
		int dayLeft = 0;
		dayLeft = dayLeftFunc(ty, sub_ky, gSN);
		if (SerialNumber != NULL)
			*SerialNumber = gSN;
		FreeLibrary(handle);
		return dayLeft;
	}
};

class ArticleDownLoader : public FileDownLoader
{
private:
	bool Download() override
	{
		HttpRequest req;
		std::wstring fullQuery= L"http://"+ConfigManager::GetInstance().GetAdvertisementServer()+
			ConfigManager::GetInstance().GetWebArticlePath();
		req.SetHost(fullQuery);
		HttpResponse res;
		HttpQuery(req, res);
		if (200 != res.statusCode)
			return false;
		auto result = res.GetBodyUtf8();
		data_.lock()->pageInfo_= nbase::UTF16ToUTF8(res.GetBodyUtf8());
		return true;
	}
};
