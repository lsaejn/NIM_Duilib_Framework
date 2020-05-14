#include "pch.h"
#include "MessageLoop.h"
#include "cef_form.h"
#include "framework.h"
#include "WinMain.h"
#include "string_util.h"
#include "templates.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
内存泄露是正常的，见LazyInstance的注释。
*/
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNUSEDPARAMS(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	// 将 bin\\cef 目录添加到环境变量，这样可以将所有 CEF 相关文件放到该目录下，方便管理
	// 在项目属性->连接器->输入，延迟加载 nim_libcef.dll
	nim_comp::CefManager::GetInstance()->AddCefDllToPath();

	HRESULT hr = ::OleInitialize(NULL);
	if (FAILED(hr))
		return 0;

	// 初始化 CEF
	CefSettings settings;
	if (!nim_comp::CefManager::GetInstance()->Initialize(nbase::win32::GetCurrentModuleDirectory() + L"resources\\", settings, kEnableOffsetRender))
	{
		return 0;
	}

	//创建守护进程,避免cef残留
	application_utily::GodBlessThisProcess();

	// 创建主线程
	MainThread thread;

	thread.setFunc([]()->auto {
		CefForm* window = new CefForm();
		window->Create(NULL, CefForm::kClassName.c_str(), WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0, nim_comp::CefManager::GetInstance()->IsEnableOffsetRender());
		window->CenterWindow();
		window->ShowWindow();
		return;
	});
	// 执行主线程循环
	thread.RunOnCurrentThreadWithLoop(nbase::MessageLoop::kUIMessageLoop);

	// 清理 CEF
	nim_comp::CefManager::GetInstance()->UnInitialize();
	::OleUninitialize();
	return 0;
}
