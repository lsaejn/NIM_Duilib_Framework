#pragma once

//没有反射的语言必然要死~~
class CefForm;
using TestFunction = std::function<void()>;
class ConsoleDebugger
{
public:
	void ConsoleForDebug(CefForm* wnd);
	void Run();
private:
	CefForm* mainWnd_;
	std::unordered_map<std::string, TestFunction> funcView;
};
