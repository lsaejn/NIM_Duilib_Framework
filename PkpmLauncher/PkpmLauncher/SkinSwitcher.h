#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <exception>
#include <vector>

#include "Alime/NonCopyable.h"



using StyleMsgCallback=std::function<void (ui::Control*)> ;
using DpiMsgCallback = std::function<void (ui::Control*)>;
DpiMsgCallback emptyFunc;

//怎么简单怎么写了,你要相信启动界面上面的东西会越来越多
//标题栏上要处理的东西也会越来越多
class Component
{
public:
	Component()
		:target_(nullptr)
	{
	}

	~Component()
	{
	}

	Component* Combine(Component* Component)
	{
		if(Component)
			target_.reset(Component);
		return this;
	}

	void ReDraw()
	{
		ReDrawSelf();
		if(target_)
			target_->ReDraw();
	}

protected:
	virtual void ReDrawSelf() = 0;
	std::shared_ptr<Component> target_;
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
		if (0==index)
			smart_ptr.reset(new BlackWrappedCaption(form));
		if (1==index)
			smart_ptr.reset(new BlueWrappedCaption(form));
		return smart_ptr;
	}

};

//fix me, 没写完
struct SkinInfoLoader
{
public:
	SkinInfoLoader()
	{
		bool ok = Init();
		UNREFERENCED_PARAMETER(ok);
	}


	bool Init()
	{
		auto pathOfSkinXml = nbase::win32::GetCurrentModuleDirectory()
			+ L"resources\\themes\\default\\cef\\caption_style.xml";
		ui::CMarkup cm;
		bool ret = cm.LoadFromFile(pathOfSkinXml.c_str());
		if (!ret) return false;
		auto rootNode = cm.GetRoot();
		defaultIndex_ = std::stoi(rootNode.GetAttributeValue(L"default_index"));
		int index = 0;
		if (!rootNode.HasChildren()) return false;
		auto theme = rootNode.GetChild();
		while (theme.IsValid())
		{
			auto arributeNode = theme.GetChild();
			while (arributeNode.IsValid())
			{
				//skinfoMap[index].push_back({ theme.GetName(),theme.GetAttributeName});
			}

			if (theme.HasSiblings())
				theme = theme.GetSibling();
			index++;
		}
		return true;
	}

	struct theme
	{
		using Arribute = std::pair<std::string, std::string>;
		int index;
		std::string themeName;
		std::vector<std::pair<std::string, std::vector<Arribute>>> arributes;
	};	
	std::vector<theme> themes;
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





