#pragma once

#include <memory>
#include <atomic>

#include "Alime/NonCopyable.h"
#include "Task.h"

//����
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
���������ǣ���ƪ�����ĵ�(Native Articles)+��ƪ�����ĵ�(Web Articles)��
��ҳ�������������ӿڻ�ȡ���������¡�
��ƪ�����ĵ�û��������ɣ���ӱ��ض�ȡ(NativeWebArticles)��
���Ǻ��������ģ���Ҳ�ǲ�ס��
*/
class IArticleReader
{
public:
	enum class ArticleType
	{
		WEB,
		NATIVE,
		NATIVE_WEB//�ӱ��ض�ȡ������������¡�
	};

	static ReadPtr GetArticleReader(ArticleType t);
	
	IArticleReader();
	~IArticleReader() = default;

	/// <summary>
	/// ����Դ�����ڴ棬������state
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// state
	/// </summary>
	bool is_good();

	/// <summary>
	/// ���ص���ԭʼ�ַ���
	/// </summary>
	std::string RawString() const;

	/// <summary>
	/// ԭʼ�ַ������ȡ���¶���
	/// </summary>
	virtual std::vector<Article> Parse() const= 0;
	
protected:
	virtual bool Read()=0;
	std::atomic<bool> state_;//bool�������ԭ�ӵ�
	std::wstring path_;//������Ϣ·��
	std::string fileContent_;
};


/// <summary>
/// ������ֱ����������������Ҫ��pool.PostTask(&WebArticleReader::Run),�ɹ���ִ��init
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
	std::string nativeCtn_;//��Ӧ�����ƪ����
	std::string webCtn_;//��Ӧ�ұ���������������
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
	void RelativeToFullPath();
};



/// <summary>
/// �ӱ��ض�ȡ����������Ŀ�����粻��/�Ͽ�/�������½���ʧ��ʱ������ֻ�ܴӱ��ض�ȡ������Ϣ
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

