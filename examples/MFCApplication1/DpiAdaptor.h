#pragma once
#include "NoCopyable.h"
#include <memory>
/*
现在的情况很微妙。
我没有想好怎么改。可能标题栏要还给网页,
也可能要处理现在的标题栏。
*/
class IAdaptor;

class IAdaptor
{
	virtual void AdaptWindow()
	{
		AdaptCaption();
		AdaptCefWindow();
	}
	virtual void AdaptCaption()=0;
	virtual void AdaptCefWindow()=0;
};


class DefaultDpiAdaptor
{
};

class DpiAdaptorFactory:public noncopyable
{
public:
	//std::shared_ptr<IAdaptor> 

private:
	DpiAdaptorFactory() = default;
	~DpiAdaptorFactory()=default;
};

