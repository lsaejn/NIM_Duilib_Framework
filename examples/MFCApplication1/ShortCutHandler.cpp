#include "pch.h"
#include "ShortCutHandler.h"


class ShortCutHandlerImpl
{
public:
	void OnAboutPkpm()
	{

	}

	void OnImprovement()
	{

	}

	void OnModelPacking()
	{

	}

	void OnRegiser()
	{

	}

	void OnContactUs()
	{

	}

	void OnIntegrityCheck()
	{

	}

	void OnParameterSettings()
	{

	}

	void OnSwitchToNetVersion()
	{

	}

	void OnUserManual()
	{

	}

	void OnUpdateOnline()
	{

	}

};

//��ɾ�����˾ɴ�����ĺ꣬�Ա����Ժ�ά���ߵĲ���
#define SHORTCUTFUNCS(className,JsName,InnerFuncs) \
	funcMap_.insert(std::make_pair(##JsName,&className::InnerFuncs));


ShortCutHandler::ShortCutHandler()
	:impl_(new ShortCutHandlerImpl)
{
	Init();
}

void ShortCutHandler::Init()
{
	SHORTCUTFUNCS(ShortCutHandlerImpl, "����PKPM", OnAboutPkpm)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "�Ľ�˵��", OnImprovement)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "ģ�ʹ��", OnModelPacking)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "ע��ؼ�", OnRegiser)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "��ϵ����", OnContactUs)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "�����Լ��", OnIntegrityCheck)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "��������", OnParameterSettings)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "��������", OnSwitchToNetVersion)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "�û��ֲ�", OnUserManual)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "���߸���", OnUpdateOnline)
}

void ShortCutHandler::CallFunc(const std::string& cutName)
{
	if (Contains(cutName))
		(impl_.get()->*funcMap_[cutName])();
	else
		throw std::invalid_argument("invalid event name");
}

bool ShortCutHandler::Contains(const std::string& cutName)
{
	return funcMap_.find(cutName) != funcMap_.end();
}
