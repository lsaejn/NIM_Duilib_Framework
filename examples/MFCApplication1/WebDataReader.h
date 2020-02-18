#pragma once

#include <unordered_map>
/*
前端要的菜单一次性加载到这里。
注意，数据是ansi的。
*/
class WebDataReader
{
public:
	void init();
	const std::string readAll() const;
	//不返回引用
	const std::string readSpecific(const std::string& filePath) const;
	//<fileName,content in u8>
	typedef std::unordered_map<std::string, std::string> XmlDataPair;
private:
	void load();
	XmlDataPair xmlData_;
};

