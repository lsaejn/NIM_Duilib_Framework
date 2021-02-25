#pragma once

#include <memory>
#include <atomic>

#include "Alime/NonCopyable.h"
#include "Task.h"

//备用
struct Article
{
	std::wstring title_;
	std::wstring text_;
	std::wstring url_;
	std::wstring imgSrc;
	int priority = 0;
};



class IArticleReader;
using ReadPtr = std::shared_ptr<IArticleReader>;


/*
最初的设计是，两篇本地文档(Native Articles)+两篇推送文档(Web Articles)。
网页调用两个单独接口获取这两种文章。
两篇推送文档没有下载完成，则从本地读取(NativeWebArticles)。
但是后来需求大改，我也记不住。
*/
class IArticleReader
{
public:
	enum class ArticleType
	{
		WEB,
		NATIVE,
		NATIVE_WEB//从本地读取代替的网络文章。
	};

	static ReadPtr GetArticleReader(ArticleType t);
	
	IArticleReader();
	~IArticleReader() = default;

	/// <summary>
	/// 将资源读到内存，并设置state
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// state
	/// </summary>
	bool is_good();

	/// <summary>
	/// 下载到的原始字符串
	/// </summary>
	std::string RawString() const;

	/// <summary>
	/// 原始字符串里读取文章对象
	/// </summary>
	virtual std::vector<Article> Parse() const= 0;
	
protected:
	virtual bool Read()=0;
	std::atomic<bool> state_;//bool本身就是原子的
	std::wstring path_;//文章信息路径
	std::string fileContent_;
};


/// <summary>
/// 这个类简直就是垃圾。子类需要先pool.PostTask(&WebArticleReader::Run),成功后执行init
/// </summary>
class WebArticleReader: public IArticleReader, QueryTask
{
public:
	WebArticleReader();
	virtual std::vector<Article> Parse() const override;
	virtual void Init() override;
	void Run(Functor after) override;
	std::string GetNativeArticles();
	std::string GetWebArticles();
private:
	virtual bool Read() override;
	ArticleDownLoader download_;
	std::string nativeCtn_;//对应左侧两篇垃圾
	std::string webCtn_;//对应右边两个滚动的垃圾
};



/// <summary>
/// 从本地读取文章信息，这些文章/图片通常随版本更新而更新
/// </summary>
class NativeArticleReader : public IArticleReader
{
public:
	NativeArticleReader();
	virtual std::vector<Article> Parse() const override;
	virtual void Init() override;
protected:
	virtual bool Read() override;

	/// <summary>
	/// 特殊初始化。配置文件里只能写相对路径，交给网页时，要将路径改为绝对路径，并且以file协议编码
	/// </summary>
	virtual void SpecificInit();
private:
	void RelativeToFullPath();
};



/// <summary>
/// 从本地读取网络文章条目。网络不佳/断开/网络文章解析失败时，程序只能从本地读取文章信息
/// </summary>
class NativeWebArticleReader : public NativeArticleReader
{
public:
	NativeWebArticleReader() = default;
protected:
	virtual void SpecificInit() override;
private:
	void EraseNativeArticles();
};

