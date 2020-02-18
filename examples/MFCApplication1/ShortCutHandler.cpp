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

//我删掉了了旧代码里的宏，以避免以后维护者的不适
#define SHORTCUTFUNCS(className,JsName,InnerFuncs) \
	funcMap_.insert(std::make_pair(##JsName,&className::InnerFuncs));


ShortCutHandler::ShortCutHandler()
	:impl_(new ShortCutHandlerImpl)
{
	Init();
}

void ShortCutHandler::Init()
{
	SHORTCUTFUNCS(ShortCutHandlerImpl, "关于PKPM", OnAboutPkpm)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "改进说明", OnImprovement)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "模型打包", OnModelPacking)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "注册控件", OnRegiser)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "联系我们", OnContactUs)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "完整性检查", OnIntegrityCheck)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "参数设置", OnParameterSettings)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "锁码设置", OnSwitchToNetVersion)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "用户手册", OnUserManual)
	SHORTCUTFUNCS(ShortCutHandlerImpl, "在线更新", OnUpdateOnline)
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
