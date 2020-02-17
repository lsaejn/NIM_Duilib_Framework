#pragma once
#include "Resource.h"
#ifdef _WIN32_WCE
#error "Windows CE 不支持 CDHtmlDialog。"
#endif 
#include <vector>
#include <string>
#include "BoundedQueue.h"



class CHtmlDlg : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CHtmlDlg)
public:
	enum { IDD = IDD_HTMLDLG};

public:
	CHtmlDlg(CWnd* pParent = NULL);
	virtual ~CHtmlDlg();

public:
	void OnClickedMin();
	void OnClickedClose();
	void OnClickedMax();
	void CheckProjectName();

	std::string OnGetSelectedPrj();
	//deprecated
	void OnMoveWindow(VARIANT);
	/// <summary>
	/// makes a page element work like title bar
	/// </summary>
	HRESULT OnMoveActive(IHTMLElement *pElement);
	/// <summary>
	/// inject your common script here
	/// </summary>
	virtual void OnNavigateComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	void OnInjectScript();
	/// <summary>
	/// open URL
	/// </summary>
	void NavigateTo(LPCTSTR pszUrl);
	/// <summary>
	/// makes the page call func like this :window.external.cppFunc()
	/// </summary>
	HRESULT IsExternalDispatchSafe(IHTMLElement *pElement);
	/// <summary>
	/// same as above
	/// </summary>
	virtual BOOL CanAccessExternal();
	/// <summary>
	/// bmp是否存在
	/// </summary>
	bool isSnapShotExist(const std::string& path);
	/// <summary>
	/// path是否有效
	/// </summary>
	///<param name="timestamp">path有效,则返回时间</param >
	///<return>path是否有效</return>
	bool getPrjInfo(const std::string& path, std::string& timestamp,const char* surfix = "buildUp.bmp");
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
	DECLARE_DISPATCH_MAP()
public:
	/// <summary>
	/// 本函数在winXp下也有效,wow!
	/// </summary>
	HRESULT STDMETHODCALLTYPE ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
	/// <summary>
	/// 用户最后的菜单选择
	/// </summary>
	/// <returns>三个索引号，见pkpm.ini下的[Html]</returns>
	BSTR OnGetDefaultMenuSelection();
	/// <summary>
	/// 网页有一次机会保存用户最后的菜单选择
	/// </summary>
	/// <param name="selections">菜单选择字符串</param>
	void OnSetDefaultMenuSelection(BSTR selections);
	//怎么会有这个函数?
	void OnSetDefaultMenuSelection2(BSTR selections);
	//得到用户当前选择的索引
	std::vector<std::pair<std::string, std::string>> MenuSelectionOnHtml();
	/// <summary>
	/// 前端程序员擅作主张地把一级菜单存在了本地，却又不处理二三级菜单
	/// 本函数将次级菜单的内容传递给网页。
	/// </summary>
	/// <param name="filename">配置文件路径</param>
	/// <returns>json格式的配置文件内容</returns>
	BSTR OnGetConfig(BSTR);
	/// <summary>
	/// 网页点击"新建"
	/// </summary>
	/// <returns>用户选择的路径</returns>
	BSTR OnNewProject();
	/// <summary>
	/// 读取工程信息
	/// </summary>
	/// <param name="fileName">配置文件cfg/pkpm.ini</param>
	/// <returns>工作/图片路径，创建时间</returns>
	BSTR ReadWorkPaths(BSTR fileName);
	/// <summary>
	/// 传递给main的最终字符串
	/// </summary>
	/// <param name="fileName"></param>
	/// <param name="fileName"></param>
	void SetPkpmPara(BSTR path,BSTR core);
	/// <summary>
	/// 双击工程的响应函数
	/// </summary>
	/// <param name="prjPath">工程路径</param>
	/// <param name="PathOfCore">exe路径</param>
	/// <param name="coreWithPara">参数</param>
	void OnDbClickProject(BSTR prjPath, BSTR PathOfCore, BSTR coreWithPara, BSTR secondMenu, BSTR thirdMenu); //单击留给网页处理
	//列表被双击/单击
	void OnListMenu(BSTR PathOfCore, BSTR coreWithPara, BSTR secondMenu, BSTR thirdMenu);

	void OnOpenDocument(BSTR RootInRibbon, BSTR filePath);
	/// <summary>
	/// 快捷键响应函数
	/// </summary>
	/// <param name="menuName">菜单名</param>
	void OnShortCut(const char* menuName);
	/// <summary>
	/// "数据转换"响应函数
	/// </summary>
	/// <param name="prjName">菜单名</param>
	void DataFormatTransfer(BSTR);
	/// <summary>
	/// 增加工程
	/// </summary>
	/// <param name="newProj">新工程名</param>
	/// <param name="filename">欲写的配置文件</param>
	/// <returns>是否添加成功</returns>
	bool AddWorkPaths(const std::string &newProj,const std::string& filename);
	/// <summary>
	/// 保存工程路径到配置文件。之前是全局函数
	/// </summary>
	/// <param name="projs">工程名</param>
	/// <param name="filename">配置文件路径</param>
	void SaveWorkPaths(collection_utility::BoundedQueue<std::string>& projs, const std::string& filename);
	/// <summary>
	/// 响应用户点击"打开"
	/// </summary>
	/// <param name="prjName"></param>
	BSTR OnOpenProject();
	/// <summary>
	/// 右键单击工程图标
	/// </summary>
	/// <param name="prjName"></param>
	void OnRightClickProject(BSTR prjName);
	/// <summary>
	/// 标题栏双击的辅助函数
	/// </summary>
	/// <returns>是否全屏</returns>
	///<remarks>should be private</remarks>
	bool IsFullScreen();
	/// <summary>
	/// 网页调用本函数，让本地有机会得到ie版本
	/// </summary>
	void SetIeVersion(BSTR);
	/// <summary>
	/// 禁止网页快捷键
	/// </summary>
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);
	/// <summary>
	/// 文档加载之后，才可以用com对象访问页面元素
	/// </summary>
	virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	/// <summary>
	/// 没时间写
	/// </summary>
	///<remarks>should be private</remarks>
	bool CommandFliter(CString path,CString core, CString secMenu, CString trdMenu);
	void OnClickedResume();
	void SwitchSrcOfZoomAll(bool maxFlag);
	std::string GetIdByPos(const CPoint cp);
	bool VersionPage();
	BSTR TellMeNewVersionExistOrNot();
	BSTR TellMeAdvertisement();

	//快捷键响应函数
	void OnBnClickedRadSingle();
	void OnBnClickedRadNet();
	void OnDropFiles(HDROP hDropInfo);//为了IE兼容，需要禁止拖拽
	void OnAboutPkpm();
	void OnImprovement();
	void OnModelPacking();
	void OnParameterSettings();
	void OnRegiser();
	void OnUserManual();
	void OnContactUs();
	void OnSwitchToNetVersion();
	void OnIntegrityCheck();
	void OnUpdateOnline();

private:
	int parseHtmlText();
	void OnInit_gMultLangStrings();
	void run_cmd( const CString&moduleName,const CString&appName1,const CString&appName2 );
	void append_work_path(const CString&path);
	bool SetCfgPmEnv();
	bool ModifyRegistry();//to make IE9&Above happy
	void OnBnClickedBtnSetLock();
	void LoadPkpmXXXXIniConfig();
	void PathFromClipBoard();
	void ReadPrivateProfile();
	void OnBnClickedBtnFileMgr();//数据打包->"模型打包"
	bool GetAppPathByCFGPATHMarker(CString strMarker,CString& retPath);
	std::vector<std::string> FindVersionFiles(const char* path, const char* prefix, const char* suffix);
	void getLatestVersion(std::vector<std::string> &result, int& major_version, int&minor_version, int&sub_version);
	void MaxBrowser();
	void OnResume();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnNcHitTest(CPoint );
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	void SaveMenuSelection(bool savePrjIndex=true);
	BOOL JavaScriptFuncInvoker(const CString strFunc, _variant_t* pVarResult);
	BOOL JavaScriptFuncInvoker(const CString strFunc, const CString strArg1, _variant_t* pVarResult);
	BOOL InvokeJsFunc(const CString strFunc, const CStringArray& paramArray, _variant_t* pVarResult);
private:
	std::string PageInfo;
	bool documentLoaded;
	bool m_bNetEdition;
	CString m_cfg_path;
	CString m_help_file;
	CString m_ver_info_exe;
	CString m_sOnlineAsk;
	CString m_sWebWiki;
	CString m_configdll_filename;
	CString m_strNameOfRegCtrl;
	CString m_strNameOfIntegrity;
	CString m_strNameOfPManager;
	CString m_strDlgCap;
	CString m_strLangType;
	CString m_strCodeType;
	int m_nMaxWorkPathNum;
	std::vector<CString>m_arrWorkPath;
	TCHAR m_sVersion_PKPM[32];
	bool m_bForceAllowUpdate;
	CString m_WorkPath;//一种"致敬"
	CString m_introductionUrl;
	int maxPrjNum;
	int miniCx_;
	int miniCy_;
	collection_utility::BoundedQueue<std::string> prjPaths_;
	CRect rectForResume;
	CRect rectCached;//for full window
	CRect rectForActive;
	CString m_coreWithPara;
	CString m_pathOfCore;
};