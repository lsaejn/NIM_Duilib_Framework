#pragma once

#include "WebDataReader.h"
#include "BoundedQueue.h"

#define WEBINTERFACE

// 控制是否使用离屏渲染，当为 true  时将使用默认窗口阴影方案，离屏渲染模式下窗口有 WS_EX_LAYERED 属性
// 当为 false 时因使用了真窗口模式不支持带有 WS_EX_LAYERED 属性窗口，所以使用外置窗口阴影方案，需要在 xml 中将窗口 shadowattached 属性设置为 false
const bool kEnableOffsetRender = true;
class CppFuncRegister;


class CefForm : public std::conditional<kEnableOffsetRender, ui::WindowImplBase, nim_comp::ShadowWndBase>::type
{
public:
	CefForm();
	~CefForm();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;
	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;
	virtual void InitWindow() override;
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	static const std::wstring	kClassName;

private:
	bool OnClicked(ui::EventArgs* msg);
	bool OnNavigate(ui::EventArgs* msg);
	void OnLoadEnd(int httpStatusCode);
	bool OnDbClicked(ui::EventArgs* msg);

	

	
	//前端读取菜单
	WEBINTERFACE std::string ReadWorkPathFromFile(const std::string& iniFileName);
	//读取工程信息
	WEBINTERFACE bool GetPrjInfo(const std::string& path, std::string& timestamp, const char* surfix = "buildUp.bmp");
	//修改标题栏
	WEBINTERFACE void SetCaption(const std::string& name);
	
	WEBINTERFACE std::string OnGetDefaultMenuSelection();

	//inner function
	void RegisterCppFuncs();
	bool IsSnapShotExist(const std::string& path);

private:
	nim_comp::CefControlBase* cef_control_;
	nim_comp::CefControlBase* cef_control_dev_;
	ui::Button*			btn_dev_tool_;
	ui::RichEdit*		edit_url_;
	ui::Label* label_;
	WebDataReader webDataReader_;
	collection_utility::BoundedQueue<std::string> prjPaths_;
	const int maxPrjNum;
};

