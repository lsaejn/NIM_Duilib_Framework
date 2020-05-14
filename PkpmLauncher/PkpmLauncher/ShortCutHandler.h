#pragma once
#include <string>
#include <memory>

#include "Alime/NonCopyable.h"

class ShortCutHandlerImpl;
typedef std::function<void()> InnerFunction;
typedef std::function<void()> CallBack;
/*
���ڿ�ݲ˵����ٱ�ʹ�õ������ҽ�ʣ�Ĺ���ҲҪ���滻
����û�г���dll�ľ����
*/
class ShortCutHandler : noncopyable
{
public:
	ShortCutHandler();
	~ShortCutHandler();
	void Init();
	void SetCallBacks(HWND, CallBack);
	void CallFunc(const std::string &funcName);
	bool Contains(const std::string& funcName) const;

private:
	ShortCutHandlerImpl* impl_;
	std::unordered_map<std::string, InnerFunction> funcMaps_;
	HWND mainWnd_;
};

