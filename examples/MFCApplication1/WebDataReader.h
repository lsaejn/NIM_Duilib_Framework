#pragma once

#include <unordered_map>
/*
ǰ��Ҫ�Ĳ˵�һ���Լ��ص����
ע�⣬������ansi�ġ�
*/
class WebDataReader
{
public:
	void Init();
	const std::string ReadAll() const;
	//����������
	const std::string ReadSpecific(const std::string& filePath) const;
	//<fileName in u8,content in u8>
	typedef std::unordered_map<std::string, std::string> XmlDataPair;
private:
	//һ���������⣬ǰ�˲�Ը�⴦��˵�����
	//����ת���Ĳ˵���Ҫ����ɾ����
	bool ModifyDataWithTag(const std::string& data, std::string& result);
	void Load();
	XmlDataPair xmlData_;
};

