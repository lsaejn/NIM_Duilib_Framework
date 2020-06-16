#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <exception>

#include "Alime/NonCopyable.h"



using StyleMsgCallback=std::function<void (ui::Control*)> ;
using DpiMsgCallback = std::function<void (ui::Control*)>;
DpiMsgCallback emptyFunc;

//怎么简单怎么写了,你要相信启动界面上面的东西会越来越多
class Component
{
public:
	Component()
		:elem_(nullptr)
	{

	}

	Component* Combine(Component* Component)
	{
		elem_ = Component;
		return this;
	}

	void ReDraw()
	{
		ReDrawSelf();
		if(elem_)
			elem_->ReDraw();
	}

protected:
	virtual void ReDrawSelf() = 0;
	Component* elem_;
};

class WrappedUiElement: public Component
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
	void ReDrawSelf()
	{
		if(ui_)
			callWhenStyleChanged_(ui_);
	}

private:
	StyleMsgCallback callWhenStyleChanged_;
	DpiMsgCallback callWhenDpiChanged_;
	ui::Control* ui_;
};

//客户端代码一起写在这里了。这就是想隐藏恶心代码的地方
class BlueWrappedCaption : public WrappedUiElement
{
public:
	BlueWrappedCaption(CefForm* form)
		:WrappedUiElement(nullptr)
	{
		auto label_ = new WrappedUiElement(form->GetCaptionLabel());
		label_->SetStrategy([](ui::Control* control) {
			control->SetAttribute(L"normaltextcolor", L"white");
			}, emptyFunc);
		auto caption_ = new WrappedUiElement(form->GetCaptionBox());
		caption_->SetStrategy([](ui::Control* control) {
			control->SetBkColor(L"blue_caption");
			}, emptyFunc);
		Combine(label_->Combine(caption_));
	}
};

class BlackWrappedCaption : public WrappedUiElement
{
public:
	BlackWrappedCaption(CefForm* form)
		:WrappedUiElement(nullptr)
	{
		auto label_ = new WrappedUiElement(form->GetCaptionLabel());
		label_->SetStrategy([](ui::Control* control) {
			control->SetAttribute(L"normaltextcolor", L"gray_caption");
			}, emptyFunc);
		auto caption_ = new WrappedUiElement(form->GetCaptionBox());
		caption_->SetStrategy([](ui::Control* control) {
			control->SetBkColor(L"gray");
			}, emptyFunc);
		Combine(label_->Combine(caption_));
	}
};

class SkinFatctory :public noncopyable
{
public:
	std::shared_ptr<WrappedUiElement> GetWrappedCaption(CefForm* form, int index)
	{
		std::shared_ptr<WrappedUiElement> smart_ptr;
		if (!index)
			smart_ptr.reset(new BlackWrappedCaption(form));
		if (index == 1)
			smart_ptr.reset(new BlueWrappedCaption(form));
		return smart_ptr;
	}

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
		static std::unordered_map<int, std::string> col = { {0,"DarkSkin"}, {1,"BlueSkin"} };
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





