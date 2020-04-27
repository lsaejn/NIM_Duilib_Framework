#pragma once
#include "Alime/NonCopyable.h"
#include <memory>

/*
现在的情况很微妙。
我没有想好怎么改。可能标题栏要还给网页,
也可能要自己处理现在的标题栏。
*/
class CefForm;

class IAdaptor
{
public:
	virtual void AdaptCaption(CefForm*)=0;
	virtual void AdaptCefWindow(CefForm*)=0;
};



class DefaultDpiAdaptor:public IAdaptor
{
public:
	DefaultDpiAdaptor() = default;

	virtual void AdaptCaption(CefForm* _window)  override;
	virtual void AdaptCefWindow(CefForm* _window) override;
};

class DpiAdaptorFactory:public noncopyable
{
public:
	//std::shared_ptr<IAdaptor> 

private:
	DpiAdaptorFactory() = default;
	~DpiAdaptorFactory()=default;
};

