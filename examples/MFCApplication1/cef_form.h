#pragma once

#include "WebDataReader.h"
#include "BoundedQueue.h"
#include "ShortCutHandler.h"
#include "AppDllAdaptor.h"
#include <atomic>

#define WEBINTERFACE
#define WM_THEME_SELECTED (WM_USER + 2)

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
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	static const std::wstring	kClassName;

private:
	/// <summary>控件点击事件,xml里面的一些控件用来调试很方便</summary>
	bool OnClicked(ui::EventArgs* msg);
	bool OnNavigate(ui::EventArgs* msg);
	void OnLoadEnd(int httpStatusCode);
	bool OnDbClicked(ui::EventArgs* msg);

	/// <summary>前端读取工程列表,旧代码,见注释</summary>
	///<param name="iniFileName">配置文件里的文件路径</param>
	///<return>工程列表的json，名称+创建时间+略缩图路径</return>
	WEBINTERFACE std::string ReadWorkPathFromFile(const std::string& iniFilePath);

	/// <summary>标题栏现在c++在处理，前端点击工程需要调用此函数</summary>
	///<param name="name">标题栏名称</param >
	WEBINTERFACE void SetCaption(const std::string& name);
	
	/// <summary>菜单记忆功能，前端调用此函数获得文件里保存的索引</summary>
	///<return>4个索引的json</return>
	WEBINTERFACE std::string OnGetDefaultMenuSelection();
	
	/// <summary>网页快捷事件</summary>
	///<param name="name">网页上快捷键的字符串</param >
	WEBINTERFACE void OnShortCut(const char* cutName);
	
	/// <summary>新建工程</summary>
	///<return>用户选择的工程路径</return>
	WEBINTERFACE std::string OnNewProject();

	/// <summary>网页左侧，数据转换调用的函数。因历史原因而存在的单独接口，参数名不准确</summary>
	///<param name="module">模块,在这里，它总是"数据转换"</param >
	///<param name="InterfaceName">接口名</param >
	///<param name="workdir">工作目录</param >
	WEBINTERFACE void DataFormatTransfer(const std::string& module, const std::string& InterfaceName, const std::string& workdir);

	/// <summary>进入工程的函数，最重要的函数</summary>
	///<param name="args">因为最初启动器是打算放到main里，所以老代码里使用了5个参数</param >
	WEBINTERFACE void OnDbClickProject(const std::vector<std::string>& args);

	/// <summary>某些三级菜单双击需要调用的函数，直接转到了OnDbClickProject</summary>
	///<param name="args">同OnDbClickProject</param >
	WEBINTERFACE void OnListMenu(const std::vector<std::string>& args);

	/// <summary>打开本地文件的接口</summary>
	///<param name="filePath">文件名以ribbon为根目录，这是历史原因</param >
	WEBINTERFACE void OnOpenDocument(const std::string& filePath);

	/// <summary>添加工程到文件</summary>
	///<param name="newProj">新工程名</param >
	///<param name="filePath">配置文件名</param >
	///<return>工程是否增加成果</return>
	WEBINTERFACE bool AddWorkPaths(const std::string& newProj, const std::string& filename);

	/// <summary>保存工程到文件,旧代码里是static函数，我也懒得再加一个函数了。</summary>
	///<param name="prjPaths">工程列表，prjPaths原本不是成员函数</param >
	///<param name="filePath">历史原因，配置文件路径</param >
	WEBINTERFACE void SaveWorkPaths(collection_utility::BoundedQueue<std::string>& prjPaths, const std::string& filePath);

	/// <summary>保存菜单索引。现在只有关闭时才保存文件。</summary>
	///<param name="filePath">json, 一二三级菜单索引+加亮工程索引</param >
	WEBINTERFACE void OnSetDefaultMenuSelection(const std::string& json_str);

	/// <summary>告知前端广告内容。</summary>
	///<return>广告内容的json，见接口文档</return>
	WEBINTERFACE std::string TellMeAdvertisement();

	/// <summary>告知前端是否存在新版本。</summary>
	///<return>bool</return>
	WEBINTERFACE bool TellMeNewVersionExistOrNot();

	/// <summary>切换到索引为index的主题</summary>
	///<param name="index">索引</param >
	void SwicthThemeTo(int index);

	/// <summary>保存用户的换肤选择</summary>
	///<param name="index">索引</param >
	void SaveThemeIndex(int index);

	/// <summary>右键工程</summary>
	///<param name="prjPath">工程路径</param >
	void OnRightClickProject(const std::wstring& prjPath);

	/// <summary>计算最新的程序版本，历史原因，cfg目录下存在类似 v4.3.4.ini 之类的文本以区分程序当前版本</summary>
	///<param name="filenames">文件的名称</param >
	///<param name="major_version">主版本号</param >
	///<param name="minor_version">副版本号</param >
	///<param name="sub_version">子版本号</param >
	void getLatestVersion(std::vector<std::string>& filenames, int& major_version, int& minor_version, int& sub_version);

	/// <summary>寻找指定格式的文件名</summary>
	///<param name="path">欲寻找的目录</param >
	///<param name="prefix">前缀</param >
	///<param name="suffix">后缀</param >
	std::vector<std::string> CefForm::FindVersionFiles(const char* path, const char* prefix, const char* suffix);

	/// <summary>获得真实路径。因为用了c++17的函数，我不确定是否有问题</summary>
	///<param name="path">原路径</param >
	///<param name="prefix">返回是否合法</param >
	///<return>真实路径</return>
	std::string PathChecker(const std::string& path, bool& legal);

	/// <summary>以线程获取广告页面</summary>
	void InitAdvertisement();
	/// <summary>获取广告页面的线程函数</summary>
	void AdvertisementThreadFunc();

	/// <summary>旧代码，名字歪了。下载广告页。</summary>
	bool VersionPage();

	/// <summary>旧代码，名字歪了。获得工程的创建时间，返回快照是否存在</summary>
	///<param name="timestamp">欲返回的时间戳</param >
	///<param name="surfix">废弃，快照放到了别的函数</param >
	///<return>要查询的文件是否有效< / return>
	bool GetPrjInfo(const std::string& path, std::string& timestamp, const char* surfix = "buildUp.bmp");

	/// <summary>注册c++函数到js</summary>
	void RegisterCppFuncs();

	/// <summary>是否存在buildup.bmp</summary>
	bool IsSnapShotExist(const std::string& path);

	/// <summary>老函数，执行dll里的函数执行命令</summary>
	void run_cmd(const CStringA& moduleName, const CStringA& appName1, const CStringA& appName2);

	/// <summary>老函数，设置PM环境变量</summary>
	bool SetCfgPmEnv();

	/// <summary>用户删掉工程文件夹，然后新建大小写错误的文件夹再打开程序。这就导致配置文件里的工程路径是一个非大小写敏感的路径</summary>
	size_t CorrectWorkPath();
	void SetHeightLightIndex(int i);
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
	std::string pageInfo_;
	std::wstring defaultCaption_;
	int indexHeightLighted_;//由于是异步通信，而很多旧代码需要直接拿加亮索引工作。你必须要出此下策。
};

