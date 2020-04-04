#pragma once
#include "NoCopyable.h"
#include <memory>
/*
���ڵ������΢�
��û�������ô�ġ����ܱ�����Ҫ������ҳ,
Ҳ����Ҫ�������ڵı�������
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

