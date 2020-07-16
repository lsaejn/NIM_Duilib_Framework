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
	/// ����Դ�����ڴ棬������state
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// �Ƿ�ɹ�����
	/// </summary>
	bool is_good();

	/// <summary>
	/// ���ص���ԭʼ�ַ���
	/// </summary>
	/// <returns></returns>
	std::string RawString() const;

	/// <summary>
	/// ԭʼ�ַ������ȡ���¶���
	/// </summary>
	/// <returns></returns>
	virtual std::vector<Article> Parse() const= 0;
	
protected:
	virtual bool Read()=0;
	std::atomic<bool> state_;//bool�������ԭ�ӵ�
	std::wstring path_;//������Ϣ·��
	std::string fileContent_;
};

/// <summary>
/// ������ֱ��������������, ����Ҫ��ִ���̣߳�Ȼ��init,Ȼ��GetRawString
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
/// �ӱ��ض�ȡ������Ϣ����Щ����/ͼƬͨ����汾���¶�����
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
	/// �����ʼ���������ļ���ֻ��д���·����������ҳʱ��Ҫ��·����Ϊ����·����������fileЭ�����
	/// </summary>
	virtual void SpecificInit();
private:
	void RelativeToReadPath();
};

/// <summary>
/// �ӱ��ض�ȡ����������Ŀ�����粻��/�Ͽ�ʱ������ֻ�ܴӱ��ض�ȡ������Ϣ
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

