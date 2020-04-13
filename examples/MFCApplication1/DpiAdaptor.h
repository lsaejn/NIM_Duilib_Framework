#pragma once
#include "Alime/NonCopyable.h"
#include <memory>

/*
���ڵ������΢�
��û�������ô�ġ����ܱ�����Ҫ������ҳ,
Ҳ����Ҫ�Լ��������ڵı�������
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

