#pragma once

#include "WebDataReader.h"
#include "BoundedQueue.h"
#include "ShortCutHandler.h"
#include "AppDllAdaptor.h"
#include <atomic>

#define WEBINTERFACE

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
	//修改标题栏
	WEBINTERFACE void SetCaption(const std::string& name);
	//获取菜单选择
	WEBINTERFACE std::string OnGetDefaultMenuSelection();
	//快捷键处理
	WEBINTERFACE void OnShortCut(const char* cutName);
	//新建工程
	WEBINTERFACE std::string OnNewProject();
	//打开工程图标
	WEBINTERFACE void OnRightClickProject(const std::wstring& prjName);
	//数据转换
	WEBINTERFACE void DataFormatTransfer(const std::string& module, const std::string& exeName, const std::string& workdir);

	WEBINTERFACE void OnDbClickProject(const std::vector<std::string>& args);
	WEBINTERFACE void OnListMenu(const std::vector<std::string>& args);
	WEBINTERFACE void OnOpenDocument(const std::string& filePath);
	WEBINTERFACE bool AddWorkPaths(const std::string& newProj, const std::string& filename);
	WEBINTERFACE void SaveWorkPaths(collection_utility::BoundedQueue<std::string>& prjPaths, const std::string& filename);
	WEBINTERFACE void OnSetDefaultMenuSelection(const std::string& json_str);
	WEBINTERFACE std::string TellMeAdvertisement();
	WEBINTERFACE bool TellMeNewVersionExistOrNot();

	void getLatestVersion(std::vector<std::string>& result, int& major_version, int& minor_version, int& sub_version);
	std::vector<std::string> CefForm::FindVersionFiles(const char* path, const char* prefix, const char* suffix);
	std::string PathChecker(const std::string& path, bool& legal);
	void InitAdvertisement();
	void AdvertisementThreadFunc();
	bool VersionPage();

	//读取工程信息
	bool GetPrjInfo(const std::string& path, std::string& timestamp, const char* surfix = "buildUp.bmp");
	//inner function
	void RegisterCppFuncs();
	bool IsSnapShotExist(const std::string& path);
	void run_cmd(const CStringA& moduleName, const CStringA& appName1, const CStringA& appName2);
	bool SetCfgPmEnv();

private:
	nim_comp::CefControlBase* cef_control_;
	nim_comp::CefControlBase* cef_control_dev_;
	ui::Button*			btn_dev_tool_;
	ui::RichEdit*		edit_url_;
	ui::Label* label_;
	WebDataReader webDataReader_;
	const int maxPrjNum_;
	collection_utility::BoundedQueue<std::string> prjPaths_;
	ShortCutHandler shortCutHandler_;
	AppDllAdaptor appDll_;
	std::mutex lock_;//fix me改成原子变量后，锁可以去掉了
	std::atomic<bool> isWebPageAvailable_;
	std::string PageInfo_;
};

