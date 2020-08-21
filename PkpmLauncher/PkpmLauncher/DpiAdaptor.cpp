#include "pch.h"
#include "DpiAdaptor.h"
#include "cef_form.h"
#pragma warning (disable: 4702) 

UINT IAdaptor::GetDpi()
{
	UINT dpi = ui::DpiManager::GetMainMonitorDPI();
	return dpi;
}

double IAdaptor::GetScale()
{
	auto dpi = GetDpi();
	auto scale = MulDiv(dpi, 100, 96) / 100.0;
	return scale;
}

std::wstring IAdaptor::SelectSkinFile()
{
	return ConfigManager::GetInstance().GetSkinFile();
}

/*
使用网易的dpi可以省掉很多工作量，我们只需要选择加载的图片即可。
不过这还是太麻烦了。
Fix me 。本函数暂时未启用。
*/
void DefaultDpiAdaptor::AdaptCaption(CefForm* _window)
{
	return;
	auto label = _window->GetCaptionLabel();
	UNREFERENCED_PARAMETER(label);
	auto vistual_caption_ = _window->GetCaptionBox();

	UINT dpi = ui::DpiManager::GetMainMonitorDPI();
	auto scale = MulDiv(dpi, 100, 96);
	double rate = scale / 100.0;

	RECT rc;
	GetWindowRect(_window->GetHWND(), &rc);
	int width = static_cast<int>((rc.right - rc.left) * rate);
	int height = static_cast<int>((rc.bottom - rc.top) * rate);
	SetWindowPos(_window->GetHWND(), HWND_TOP, 0, 0, width,
		height, SWP_NOMOVE);
	//_window->SetMinInfo();
	//_window->SetMaximizeInfo({})
	auto captionHeight = vistual_caption_->GetFixedHeight();
	auto captionWidth = vistual_caption_->GetFixedWidth();
	vistual_caption_->SetAttribute(L"height", std::to_wstring(captionHeight * rate));
	rc = _window->GetCaptionRect();
	auto rect = ui::UiRect(0, 0, static_cast<int>(captionWidth * rate), static_cast<int>(captionHeight * rate));
	_window->SetCaptionRect(rect);
}

 void DefaultDpiAdaptor::AdaptCefWindow(CefForm* _window)
{
	auto* cef = _window->GetCef();
	cef->SetZoomLevel(static_cast<float>(GetZoomLevel()));
}

 double DefaultDpiAdaptor::GetZoomLevel()
 {
	 auto scale = GetScale();
	 ///see www.magpcss.org/ceforum/viewtopic.php?f=6&t=11491
	 auto level = log(scale) / log(1.2);
	 return level;
 }

 std::wstring DefaultDpiAdaptor::SelectSkinFile()
 {
	 auto scale = MulDiv(GetDpi(), 100, 96);
	 auto defaultFile=IAdaptor::SelectSkinFile();
	 if (scale < 125)
		 return defaultFile;
	 else if (scale >= 125 && scale <= 350)
	 {
		 return SkinFileAsDpi(defaultFile,L"2k");
	 }		 
	 else
	 {
		 return SkinFileAsDpi(defaultFile, L"4k");
	 }		 
 }

 std::wstring DefaultDpiAdaptor::SkinFileAsDpi(const std::wstring& file,
	 const std::wstring& suffix)
 {
	 auto i=file.rfind('.');
	 auto result = file.substr(0, i) + suffix + file.substr(i);
	 return result;
 }
