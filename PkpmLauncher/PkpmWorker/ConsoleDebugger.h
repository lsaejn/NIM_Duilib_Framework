#pragma once

//û�з�������Ա�ȻҪ��~~
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
