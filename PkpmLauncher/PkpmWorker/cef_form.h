#pragma once

#include "WebDataReader.h"
#include "BoundedQueue.h"
#include "ShortCutHandler.h"
#include "AppDllAdaptor.h"
#include "DpiAdaptor.h"
#include "Task.h"
#include "Article.h"
#include "AppTray.h"

#include "Alime/countDownLatch.h"
#include "Alime/ExecutorService.h"
#include <atomic>
#include <mutex>



const bool kEnableOffsetRender = true;

class CefForm : public std::conditional<kEnableOffsetRender, ui::WindowImplBase, nim_comp::ShadowWndBase>::type
{
public:
	CefForm();
	~CefForm();
	
	/// <summary>设置资源文件夹</summary>
	virtual std::wstring GetSkinFolder() override;

	/// <summary>设置界面文件</summary>
	virtual std::wstring GetSkinFile() override;

	/// <summary>设置类名</summary>
	virtual std::wstring GetWindowClassName() const override;

	/// <summary>cefcontrol在xml是自定义类型,自己创建对象</summary>
	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;
	virtual void InitWindow() override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual LRESULT OnNcLButtonDbClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	
private:

	/// <summary>控件点击事件,xml里面的一些控件用来调试很方便</summary>
	bool OnClicked(ui::EventArgs* msg);
	void OnLoadEnd(int httpStatusCode);
	void InitUiVariable();
	void AttachFunctionToShortCut();
	void AppendThreadTask();
	/// <summary>前端读取工程列表,旧代码,见注释</summary>
	///<param name="iniFileName">配置文件里的文件路径</param>
	///<return>工程列表的json，名称+创建时间+略缩图路径</return>
	std::string ReadWorkPathFromFile(const std::string& iniFilePath);

	/// <summary>标题栏现在c++在处理，前端点击工程需要调用此函数</summary>
	///<param name="name">标题栏名称</param >
	void SetCaptionWithProjectName(const std::string& prjname);
	
	/// <summary>菜单记忆功能，前端调用此函数获得文件里保存的索引</summary>
	///<return>4个索引的json</return>
	std::string OnGetDefaultMenuSelection();
	
	/// <summary>网页快捷事件</summary>
	///<param name="name">网页上快捷键的字符串</param >
	void OnShortCut(const std::wstring& cutName);
	
	/// <summary>新建工程</summary>
	///<return>用户选择的工程路径</return>
	std::string OnNewProject();

	/// <summary>数据转换。新版本里已废弃，但保留此接口，可以让程序向下兼容(前提是使用新配置文件)</summary>
	///<param name="module">模块,在这里，它总是"数据转换"</param >
	///<param name="InterfaceName">接口名</param >
	///<param name="workdir">工作目录</param >
	void DataFormatTransfer(const std::string& module, const std::string& InterfaceName, const std::string& workdir);

	/// <summary>进入工程的函数，最重要的函数</summary>
	///<param name="args">因为最初启动器是打算放到main里，所以老代码里使用了5个参数</param >
	void OnDbClickProject(const std::vector<std::string>& args);

	/// <summary>某些三级菜单双击需要调用的函数，直接转到了OnDbClickProject</summary>
	///<param name="args">同OnDbClickProject</param >
	void OnListMenu(const std::vector<std::string>& args);

	/// <summary>打开本地文件的接口, 给用户手册和更新说明</summary>
	///<param name="filePath">文件名以ribbon为根目录，这是历史原因</param >
	void OnOpenDocument(const std::string& filePath);

	/// <summary>添加工程到文件</summary>
	///<param name="newProj">新工程名</param >
	///<param name="filePath">配置文件名</param >
	///<return>工程是否增加成功</return>
	bool AddWorkPaths(const std::string& newProj, const std::string& filename);

	/// <summary>保存工程到文件,旧代码里是static函数，我也懒得再加一个函数了。</summary>
	///<param name="prjPaths">工程列表，prjPaths原本不是成员函数</param >
	///<param name="filePath">历史原因，配置文件路径</param >
	void SaveWorkPaths(collection_utility::BoundedQueue<std::string>& prjPaths, const std::string& filePath);

	/// <summary>保存菜单索引。现在只有关闭时才保存文件。</summary>
	///<param name="filePath">json, 一二三级菜单索引+加亮工程索引</param >
	void OnSetDefaultMenuSelection(const std::string& json_str);

	/// <summary>告知前端广告内容。</summary>
	///<return>广告内容的u8json，见接口文档</return>
	std::string TellMeAdvertisement();

	/// <summary>
	/// 告知前端是否存在新版本或新补丁，需求疑有误，
	/// 有补丁必有更新包，那么根本没必要检测补丁。
	/// </summary>
	///<return>bool</return>
	bool TellMeNewVersionExistOrNot();

	/// <summary>标题栏切换到索引为index的主题</summary>
	///<param name="index">索引</param >
	///<return>是否成功</return>
	bool SwicthThemeTo(int index);

	/// <summary>保存用户的换肤选择</summary>
	///<param name="index">索引</param >
	/// <param name="name">名称</param >
	void SaveThemeIndex(int index, const std::wstring& name);

	/// <summary>右键工程</summary>
	///<param name="prjPath">工程路径</param >
	void OnRightClickProject(const std::wstring& prjPath);

	/// <summary>获取大小写敏感的路径</summary>
	///<param name="path">原路径</param >
	///<param name="prefix">返回是否合法</param >
	///<return>真实路径</return>
	std::string PathChecker(const std::string& path, bool& legal);

	/// <summary>旧代码，名字歪了。获得工程的创建时间，返回快照是否存在</summary>
	///<param name="timestamp">欲返回的时间戳</param >
	///<param name="surfix">废弃，快照放到了别的函数</param >
	///<return>要查询的工程是否有效< / return>
	bool GetPrjInfo(const std::string& path, std::string& timestamp, const char* surfix = "buildUp.bmp");

	/// <summary>注册c++函数到js</summary>
	void RegisterCppFuncs();

	///<summary>加载时调用的函数</summary>
	void OnLoadStart();

	/// <summary>是否存在buildup.bmp</summary>
	///<param name="path">工程路径</param >
	bool IsSnapShotExist(const std::string& path);

	/// <summary>老函数，执行dll里的函数执行命令。张磊的杰作</summary>
	///<param name="moduleName">二级菜单</param >
	///<param name="appName1">三级菜单</param >
	///<param name="appName2">基本上没有</param >
	void run_cmd(const std::string& moduleName, const std::string& appName1, const std::string& appName2);

	/// <summary>老函数，设置环境变量</summary>
	bool SetEnv();

	/// <summary>用户删掉工程文件夹，然后新建大小写错误的文件夹再打开程序。这就导致配置文件里的工程路径是一个非大小写敏感的路径</summary>
	///<return>有效的工程数量</return>
	size_t CorrectWorkPath();

	/// <summary>设置当前高亮的索引</summary>
	///<param name="index">索引编号</param >
	void SetHeightLightIndex(const int index);

	/// <summary>对话框接受文件拖拽</summary>
	void EnableAcceptFiles();

	/// <summary>废弃,换肤按钮的事件</summary>
	void AttachClickCallbackToSkinButton();

	/// <summary>初始化日志库</summary>
	void InitSpdLog();

	/// <summary>打开并打爆装配式</summary>
	void OpenBimExe();

private:
	/// <summary></summary>
	void CheckVersionInDocument(const rapidjson::Document& document,
		bool& hasNewVersion, bool& hasNewPatch);

public:
	void AcceptDpiAdaptor(IAdaptor* acc);
	nim_comp::CefControlBase* GetCef();
	ui::Label* GetCaptionLabel();
	ui::HBox* GetCaptionBox();

public:
	static const std::wstring	kClassName;

private:
	ui::Label* label_;
	ui::Button* skinSettings_;
	ui::HBox* vistual_caption_;
	ui::Window* this_window_;
	
	WebDataReader webDataReader_;
	WebArticleReader webArticleReader_;
	WebPageDownLoader webPageData_;

	nim_comp::CMenuWnd* pMenu_;
	nim_comp::CefControlBase* cef_control_;
	nim_comp::CefControlBase* cef_control_dev_;
	
	AppTray tray_;
	const int maxPrjNum_;
	AppDllAdaptor appDll_;
	int indexHeightLighted_;
	std::wstring defaultCaption_;
	Alime::ExecutorService pool_;
	ShortCutHandler shortCutHandler_;
	collection_utility::BoundedQueue<std::string> prjPaths_;
};

