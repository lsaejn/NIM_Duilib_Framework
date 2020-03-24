#pragma once
#include <string>
#include <unordered_map>

#include <iostream>
#include <map>
#include <memory>
#include <functional>

#include "NoCopyable.h"


/*
这个.h 并非良好的示范。
你总是关心面向对象和impl, 出来的就是一堆屎。
这个头文件的正常使用是建立在下面的基础:
1.对于同一个编译单元内的全局变量,它们初始化的顺序与声明的顺序一致
2.
*/
struct SkinInfo
{
	std::string captionBkColor_;
	std::string normaltextcolor_;
};

class SkinInfoManager
{
public:
	SkinInfoManager& Get()
	{
		static SkinInfoManager sm;
		return sm;
	}
private:
	SkinInfoManager()
	{
		auto pathOfSkinXml = nbase::win32::GetCurrentModuleDirectory() + L"resources\\themes\\default\\cef\\caption_style.xml";
		ui::CMarkup cm;
		cm.LoadFromFile(pathOfSkinXml.c_str());
		auto rootNode = cm.GetRoot();
		defaultIndex_ =std::stoi( rootNode.GetAttributeValue(L"default_index"));
		if (rootNode.HasChildren())
		{
			auto child = rootNode.GetChild();
		}
	}
	std::unordered_map<std::string, SkinInfo> skinfoMap;
	int defaultIndex_;
};

class SkinSwitcher
{
public:
	virtual ~SkinSwitcher() = default;
	virtual void Switch(nim_comp::Box* b, ui::Label* label) = 0;
	std::string skinName_;
};

using Function = std::function<SkinSwitcher * (void)>;

class SkinSwitcherFatctory:public noncopyable
{
public:
	SkinSwitcherFatctory& Instance()
	{
		static SkinSwitcherFatctory fac;
		return fac;
	}
	static std::unordered_map<std::string, Function> SkinThemes_;
	static std::shared_ptr<SkinSwitcher> SkinSwitcherFatctory::Get(const int index)
	{
		static std::unordered_map<int, std::string> col = { {0,"LightSkin"},{1,"DarkSkin"} };
		const std::string name=col[index];
		auto iter = SkinThemes_.find(name);
		if (iter != SkinThemes_.end())
		{
			return std::shared_ptr<SkinSwitcher>(iter->second());
		}
		return nullptr;
	}
};
std::unordered_map<std::string, Function> SkinSwitcherFatctory::SkinThemes_;

class Register
{
public:
	Register(const std::string& className, Function&& constructor)
	{
		SkinSwitcherFatctory::SkinThemes_.insert({ className, constructor });
		std::cout << "insert" << className << std::endl;
	}
};

#define REGISTER(CLASS_NAME) \
const Register Registers##CLASS_NAME(#CLASS_NAME, []()\
{\
    return new CLASS_NAME(); \
});

class DarkSkin :public SkinSwitcher
{
public:
	virtual ~DarkSkin()
	{
		std::cout << "decons DarkSkin" << std::endl;
	}
	virtual void Switch(nim_comp::Box* vistual_caption, ui::Label* _label)
	{
		vistual_caption->SetBkColor(L"gray");
		_label->SetAttribute(L"normaltextcolor", L"gray_caption");
		std::cout << "Switch to LightSkin" << std::endl;
	}
}; 
REGISTER(DarkSkin)


class LightSkin :public SkinSwitcher
{
public:
	virtual ~LightSkin()
	{
		std::cout << "decons LightSkin" << std::endl;
	}
	virtual void Switch(nim_comp::Box* vistual_caption, ui::Label* _label)
	{
		vistual_caption->SetBkColor(L"bk_wnd_lightcolor");
		_label->SetAttribute(L"normaltextcolor", L"darkcolor");
		std::cout << "Switch to LightSkin" << std::endl;
	}
}; 
REGISTER(LightSkin)





