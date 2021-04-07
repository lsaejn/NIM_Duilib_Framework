#pragma once
#include <string>
#include <memory>

#include "Alime/NonCopyable.h"

class ShortCutHandlerImpl;
typedef std::function<void()> InnerFunction;
typedef std::function<void()> CallBack;
typedef std::function<bool()> BooleanCallBack;

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
	void SetCallBacks(HWND, CallBack);//���õ���exeǰ�����ش��ڵĺ���
	void CallFunc(const std::wstring &funcName);
	bool Contains(const std::wstring& funcName) const;
	void SetNewVersionChecker(BooleanCallBack);

private:
	ShortCutHandlerImpl* impl_;
	std::unordered_map<std::wstring, InnerFunction> funcMaps_;
	HWND mainWnd_;

};
