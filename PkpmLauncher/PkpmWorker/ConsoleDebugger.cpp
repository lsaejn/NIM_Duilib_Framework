#include "pch.h"
#include "ConsoleDebugger.h"
#include "Alime/Console.h"
#include "cef_form.h"

#include <thread>
#include <unordered_map>

void ConsoleDebugger::ConsoleForDebug(CefForm* /*wnd*/)
{

}

void ConsoleDebugger::Run()
{
	Alime::Console::CreateConsole();
	RECT rc;
	GetWindowRect(mainWnd_->GetHWND(), &rc);
	Alime::Console::SetWindowPosition(rc.right - 20, rc.top + 20);
	Alime::Console::SetWindowSize(400, rc.bottom - rc.top - 28);
	Alime::Console::SetTitle(L"Alime");
	Alime::Console::SetColor(Alime::Console::CYAN);
	std::thread t([this]() {
		while (true)
		{
			auto str = Alime::Console::ReadLine();
			if (str.find(L"SetCaption") != std::wstring::npos)
			{
				auto result = str.substr(10);
				//SetCaptionWithProjectName(u8"PKPM结构设计软件 10版 V5.1.1    " + nbase::UTF16ToUTF8(result));
			}
			else if (str.find(L"Clear") != std::wstring::npos)
			{
				Alime::Console::Clear();
			}
			else if (str.find(L"Quit") != std::wstring::npos)
			{
				break;
			}
			else
			{
				Alime::Console::SetColor(1, 0, 0, 0);
				Alime::Console::WriteLine(L"没有这个接口");
				Alime::Console::SetColor(Alime::Console::CYAN);
			}
		}
		FreeConsole();
		});
	t.detach();
}