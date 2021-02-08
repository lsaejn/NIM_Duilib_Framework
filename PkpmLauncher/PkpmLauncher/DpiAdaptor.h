#pragma once
#include "Alime/NonCopyable.h"
#include "ConfigFileManager.h"
#include <memory>

/// <summary>
/// 两种适应dpi的方法。1是框架自带，但是需要自己处理图片
/// 2是准备多个xml。我们使用的是后者。
/// 这个方法都需要自己处理标题栏拖动高度和cef窗口放大系数
/// DPI的适配比想象得容易，但是图片资源总是显示锯齿。
/// </summary>

class CefForm;

class IAdaptor
{
public:
	virtual void AdaptCaption(CefForm*)=0;
	virtual void AdaptCefWindow(CefForm*)=0;
	virtual std::wstring SelectSkinFile();
	UINT GetDpi();
	double GetScale();
};

class DefaultDpiAdaptor:public IAdaptor
{
public:
	DefaultDpiAdaptor() = default;

	virtual void AdaptCaption(CefForm* _window)  override;
	virtual void AdaptCefWindow(CefForm* _window) override;
	double GetZoomLevel();
	std::wstring SelectSkinFile();
private:
	std::wstring SkinFileAsDpi(const std::wstring& file, const std::wstring& suffix);
};

class EmptyDpiAdaptor :public IAdaptor
{
public:
	EmptyDpiAdaptor() = default;

	virtual void AdaptCaption(CefForm* /*_window*/) {};
	virtual void AdaptCefWindow(CefForm* /*_window*/) {};
};

class DpiAdaptorFactory:public noncopyable
{
public:
	enum class AdaptorTy
	{
		Default,
		CaptionInclude
	};

	static std::shared_ptr<IAdaptor> GetAdaptor()
	{
		if(ConfigManager::GetInstance().IsAdaptDpiOn() ||
			ConfigManager::GetInstance().IsModifyWindowOn()
			)
			return std::make_shared<DefaultDpiAdaptor>();
		else
			return std::make_shared<EmptyDpiAdaptor>();
	}

private:
	DpiAdaptorFactory() = default;
	~DpiAdaptorFactory()=default;
};

