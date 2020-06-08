#include "pch.h"
#include "Article.h"
#include "ConfigFileManager.h"
#include "string_util.h"
#include "nlohmann/json.hpp"
#include "nlohmann/fifo_map.hpp"

using namespace nlohmann;

template<class K, class V, class dummy_compare, class A>
using my_workaround_fifo_map = fifo_map<K, V, fifo_map_compare<K>, A>;
using my_json = basic_json<my_workaround_fifo_map>;

//////////////////////  IArticleReader
ReadPtr IArticleReader::GetArticleReader(ArticleType t)
{
	if (t == ArticleType::WEB)
		return std::shared_ptr<IArticleReader>(new WebArticleReader());
	else if (t == ArticleType::NATIVE)
		return std::shared_ptr<IArticleReader>(new NativeArticleReader());
	else
		return std::shared_ptr<IArticleReader>(new NativeWebArticleReader());
}

IArticleReader::IArticleReader()
	:path_(), state_(false)
{
}

std::string IArticleReader::RawString() const
{
	return fileContent_;
}

bool IArticleReader::is_good()
{
	return state_;
}

//////////////////////  NativeArticleReader
NativeArticleReader::NativeArticleReader()
	:IArticleReader()
{	
}

std::vector<Article> NativeArticleReader::Parse() const
{
	std::vector<Article> ret;
	return ret;
}

void NativeArticleReader::Init()
{
	path_ = nbase::win32::GetCurrentModuleDirectory() + ConfigManager::GetInstance().GetNativeArticlePath();
	state_=Read();
	SpecificInit();
}
void NativeArticleReader::SpecificInit()
{
	RelativeToReadPath();
}

bool NativeArticleReader::Read()
{
	return nbase::ReadFileToString(path_, fileContent_);
}

void NativeArticleReader::RelativeToReadPath()
{
	if (!state_)
		return;
	try
	{
		nlohmann::json json = nlohmann::json::parse(fileContent_);
		std::string prefix_in_u8 = nbase::UTF16ToUTF8(nbase::win32::GetCurrentModuleDirectory());
		nlohmann::json articles = json["nativeArticles"];
		for (json::iterator it = articles.begin(); it != articles.end(); ++it)
		{
			std::string relativePath = (*it)["imgSrc"];
			std::string fullPath = "file://" + prefix_in_u8 + relativePath;
			fullPath=application_utily::FileEncode(fullPath);
			(*it)["imgSrc"]= fullPath;
		}
		json.erase("webArticles");
		fileContent_ = json.dump();
	}
	catch (...)
	{
		MessageBox(NULL,L"NativeArticleReader, Ω‚Œˆ≈‰÷√Œƒº˛¥ÌŒÛ", L"¥ÌŒÛ", MB_SYSTEMMODAL);
	}
}

///////////////////////////////////// WebArticleReader
WebArticleReader::WebArticleReader()
	:IArticleReader()
{
}

std::vector<Article> WebArticleReader::Parse() const
{
	std::vector<Article> ret;
	return ret;
}

void WebArticleReader::Init()
{
	path_ = ConfigManager::GetInstance().GetAdvertisementServer()
		+ ConfigManager::GetInstance().GetWebArticlePath();
	state_=Read();
}

void WebArticleReader::Run(Functor after)
{
	download_.Run(std::move(after));
}

//read string from download_
bool WebArticleReader::Read()
{
	bool ret= download_.data_.lock()->isWebPageCooked_;
	if (ret)
		fileContent_ = download_.data_.lock()->pageInfo_;
	return ret;
}

///////////////////NativeWebArticleReader
void NativeWebArticleReader::SpecificInit()
{
	EraseNativeArticles();
}

void NativeWebArticleReader::EraseNativeArticles()
{
	if (!state_)
		return;
	try
	{
		nlohmann::json json = nlohmann::json::parse(fileContent_);
		json.erase("nativeArticles");
		if (json.find("webArticles") == json.end())
			throw std::exception("bad json file");
		fileContent_ = json.dump();
	}
	catch (...)
	{
#ifdef DEBUG
		AfxMessageBox(L"NativeWebArticleReader, Ω‚Œˆ≈‰÷√Œƒº˛¥ÌŒÛ", MB_SYSTEMMODAL);
#endif // DEBUG
		state_ = false;
	}
}