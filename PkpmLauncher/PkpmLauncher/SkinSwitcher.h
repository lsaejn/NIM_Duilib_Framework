#pragma once
#include <string>
#include <unordered_map>
#include <iostream>
#include <map>
#include <memory>
#include <functional>
#include <exception>

#include "Alime/NonCopyable.h"



using StyleMsgCallback=std::function<void (ui::Control*)> ;
using DpiMsgCallback = std::function<void (ui::Control*)>;

class WrappedCaption : public noncopyable
{
public:
	class WrappedUiElement
	{
	public:
		WrappedUiElement(ui::Control* ui)
		{
			ui_ = ui;
		}
		void SetStrategy(StyleMsgCallback sm, DpiMsgCallback dm)
		{
			callWhenStyleChanged_ = move(sm);
			callWhenDpiChanged_ = move(dm);
		}
	private:
		StyleMsgCallback callWhenStyleChanged_;
		DpiMsgCallback callWhenDpiChanged_;
		ui::Control* ui_;
	};

public:
	WrappedCaption() = default;
	void Register(ui::Control* _control)
	{
		if (std::find(controls_.begin(), controls_.end(), _control)
			!= controls_.end())
			throw "what the fuck";
		//controls_.emplace_back(_control);
	}
	void UnRegister(ui::Control* _control)
	{
		controls_.erase(
			std::find(controls_.begin(), controls_.end(), _control));
	}
	void ResponseDpiMsg()
	{

	}
	void ResponseStyleMsg()
	{

	}
private:
	std::vector<ui::Control*> controls_;
};

class SkinInfoLoader
{
public:
	SkinInfoLoader()
	{
		auto pathOfSkinXml = nbase::win32::GetCurrentModuleDirectory()
			+ L"resources\\themes\\default\\cef\\caption_style.xml";
		ui::CMarkup cm;
		cm.LoadFromFile(pathOfSkinXml.c_str());
		auto rootNode = cm.GetRoot();
		defaultIndex_ =std::stoi( rootNode.GetAttributeValue(L"default_index"));
		if (rootNode.HasChildren())
		{
			auto child = rootNode.GetChild();
		}
	}

	std::unordered_map<std::string, std::string> skinfoMap;
	int defaultIndex_;
};

class SkinSwitcher
{
public:
	virtual ~SkinSwitcher() = default;
	virtual void Switch(nim_comp::Box* b, ui::Label* label) = 0;
	std::string skinName_;
	SkinInfoLoader skinLoader_;
};

using Function = std::function<SkinSwitcher* (void)>;

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
		static std::unordered_map<int, std::string> col = { {0,"DarkSkin"},{1,"BlueSkin"} };
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
	}
	virtual void Switch(nim_comp::Box* vistual_caption, ui::Label* _label)
	{
		vistual_caption->SetBkColor(L"gray");
		_label->SetAttribute(L"normaltextcolor", L"gray_caption");
	}
}; 
REGISTER(DarkSkin)


class BlueSkin :public SkinSwitcher
{
public:
	virtual ~BlueSkin()
	{
	}
	virtual void Switch(ui::Box* vistual_caption, ui::Label* _label)
	{
		vistual_caption->SetBkColor(L"blue_caption");
		_label->SetAttribute(L"normaltextcolor", L"white");
	}
}; 
REGISTER(BlueSkin)





