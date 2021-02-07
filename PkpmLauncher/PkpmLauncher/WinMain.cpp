#include "pch.h"
#include "MessageLoop.h"
#include "cef_form.h"
#include "framework.h"
#include "WinMain.h"
#include "utility.h"
#include "templates.h"
#include "ConfigFileManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
内存泄露是正常的，见LazyInstance的注释。通常就是两个loop指针。
另一个原因就是授权码查询函数，这个函数实在是查询时间太长了。
*/

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNUSEDPARAMS(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	nim_comp::CefManager::GetInstance()->AddCefDllToPath();

	HRESULT hr = ::OleInitialize(NULL);
	if (FAILED(hr))
		return 0;

	//ConfigManager::GetInstance().LoadConfigFile();
	CefSettings settings;
	if (!nim_comp::CefManager::GetInstance()->Initialize(nbase::win32::GetCurrentModuleDirectory() + L"resources\\", settings, kEnableOffsetRender))
	{
		return 0;
	}
	
	//_CrtSetBreakAlloc(626);
	// 创建主线程..
	MainThread thread;
	//如果你实在有洁癖，可以在这里初始化全局变量
	//但是这段代码不应该截获消息，否则子进程先于父进程初始化完成。
	thread.setFunc([]()->auto {
		CefForm* window = new CefForm();
		window->Create(NULL, CefForm::kClassName.c_str(), WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0, nim_comp::CefManager::GetInstance()->IsEnableOffsetRender());
		window->CenterWindow();
		window->ShowWindow();
		return;
	});
	thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop);
	nim_comp::CefManager::GetInstance()->UnInitialize();
	::OleUninitialize();
	return 0;
}
