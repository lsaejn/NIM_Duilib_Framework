#include "pch.h"
#include "DpiAdaptor.h"
#include "cef_form.h"


void DefaultDpiAdaptor::AdaptCaption(CefForm* _window)
{
	auto label = _window->GetCaptionLabel();
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
	_window->SetCaptionRect(
		ui::UiRect(0, 0,
			static_cast<int>(captionWidth * rate),
			static_cast<int>(captionHeight * rate)));
}

 void DefaultDpiAdaptor::AdaptCefWindow(CefForm* _window)
{
	auto* cef = _window->GetCef();
}
