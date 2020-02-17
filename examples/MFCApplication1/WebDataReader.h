#pragma once

#include <unordered_map>
/*
ǰ��Ҫ�Ĳ˵�һ���Լ��ص����
ע�⣬������ansi�ġ�
*/
class WebDataReader
{
public:
	void init();
	const std::string& readAll() const;
	//����������
	const std::string readSpecific(const std::string& filePath) const;
	typedef std::unordered_map<std::string, std::string> XmlDataPair;
private:
	void load();
	XmlDataPair xmlDatd_;
};

