#pragma once

#include <unordered_map>


class IWebDataVisitor;
 
class IDataAcceptor
{
public:
	virtual void Accept(IWebDataVisitor* visitor) = 0;
	virtual const std::string ReadSpecific(const std::string& filePath) const = 0;
};

class IWebDataVisitor
{
public:
	IWebDataVisitor(const std::string& key)
	{
		key_ = key;
	}
	virtual ~IWebDataVisitor() = default;
	virtual void Visit(IDataAcceptor* self) = 0;
	std::string key_;
	std::string value_;
};

class WebDataReader: public IDataAcceptor
{
public:
	void Init();
	const std::string ReadAll() const;
	//不返回引用
	const std::string ReadSpecific(const std::string& filePath) const override;

	virtual void Accept(IWebDataVisitor* visitor) override;
	//<fileName in u8,content in u8>
	typedef std::unordered_map<std::string, std::string> XmlDataPair;

	std::string LastQuery();
private:
	//一个遗留问题，前端不愿意处理菜单内容
	//数据转换的菜单需要我来删除掉
	bool ModifyDataWithTag(const std::string& data, std::string& result);
	void Load();
	XmlDataPair xmlData_;
	mutable std::string lastQueryFilePath_;
};


class WebDataVisitor : public IWebDataVisitor
{
public:
	WebDataVisitor(const std::string& key)
		: IWebDataVisitor(key)
	{
	}

	virtual void Visit(IDataAcceptor* self) override
	{
		value_ = self->ReadSpecific(key_);
		Encode();
	}

	std::string GetValue() const
	{
		return value_;
	}

private:
	virtual void Encode()
	{
	}
};

using WebDataVisitorInU8 = WebDataVisitor;

class WebDataVisitorInAnsi : public WebDataVisitor
{
public:
	WebDataVisitorInAnsi(const std::string& key)
		: WebDataVisitor(key)
	{
	}

	void Encode()
	{
		value_ = nbase::UnicodeToAnsi(nbase::UTF8ToUTF16(value_));
	}
};

