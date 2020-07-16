#pragma once

#include <memory>
#include <atomic>

#include "Alime/NonCopyable.h"
#include "Task.h"

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

class IArticleReader
{
public:
	enum class ArticleType
	{
		WEB,
		NATIVE,
		NATIVE_WEB
	};
	static ReadPtr GetArticleReader(ArticleType t);
	
	IArticleReader();
	~IArticleReader() = default;

	/// <summary>
	/// 将资源读到内存，并设置state
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// 是否成功下载
	/// </summary>
	bool is_good();

	/// <summary>
	/// 下载到的原始字符串
	/// </summary>
	/// <returns></returns>
	std::string RawString() const;

	/// <summary>
	/// 原始字符串里读取文章对象
	/// </summary>
	/// <returns></returns>
	virtual std::vector<Article> Parse() const= 0;
	
protected:
	virtual bool Read()=0;
	std::atomic<bool> state_;//bool本身就是原子的
	std::wstring path_;//文章信息路径
	std::string fileContent_;
};

/// <summary>
/// 这个类简直就是垃圾，垃圾, 你需要先执行线程，然后init,然后GetRawString
/// </summary>
class WebArticleReader: public  IArticleReader, QueryTask
{
public:
	WebArticleReader();
	virtual std::vector<Article> Parse() const override;
	virtual void Init() override;
	void Run(Functor after) override;
private:
	virtual bool Read() override;
	ArticleDownLoader download_;
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
	void RelativeToReadPath();
};

/// <summary>
/// 从本地读取网络文章条目。网络不佳/断开时，程序只能从本地读取文章信息
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

