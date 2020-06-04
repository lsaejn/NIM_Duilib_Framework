#pragma once

#include <memory>

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
	
	/// <summary>初始化</summary>
	/// <param name="filePath">本参数是配置文件路，而非推送文章路径</param>
	IArticleReader();
	~IArticleReader() = default;

	virtual void Init() = 0;
	bool is_good();
	std::string RawString() const;
	virtual std::vector<Article> Parse() const= 0;
	
protected:
	virtual bool Read()=0;
	bool state_;
	std::wstring path_;
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

class NativeArticleReader : public IArticleReader
{
public:
	NativeArticleReader();
	virtual std::vector<Article> Parse() const override;
	virtual void Init() override;
protected:
	virtual bool Read() override;
	virtual void SpecificInit();
private:
	void RelativeToReadPath();
};

class NativeWebArticleReader : public NativeArticleReader
{
public:
	NativeWebArticleReader() = default;
protected:
	virtual void SpecificInit() override;
private:
	void EraseNativeArticles();
};

