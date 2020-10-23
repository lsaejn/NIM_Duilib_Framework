#pragma once
#include <string>
#include <memory>

#include "Alime/NonCopyable.h"

class ShortCutHandlerImpl;
typedef std::function<void()> InnerFunction;
typedef std::function<void()> CallBack;
typedef std::function<bool()> BooleanCallBack;

/*
由于快捷菜单很少被使用到，并且仅剩的功能也要被替换
程序并没有持有dll的句柄。
*/
class ShortCutHandler : noncopyable
{
public:
	ShortCutHandler();
	~ShortCutHandler();
	void Init();
	void SetCallBacks(HWND, CallBack);//设置调用exe前后隐藏窗口的函数
	void CallFunc(const std::string &funcName);
	bool Contains(const std::string& funcName) const;
	void SetNewVersionChecker(BooleanCallBack);

private:
	ShortCutHandlerImpl* impl_;
	std::unordered_map<std::string, InnerFunction> funcMaps_;
	HWND mainWnd_;

};

