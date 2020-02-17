#pragma once
#include "Resource.h"
#ifdef _WIN32_WCE
#error "Windows CE ��֧�� CDHtmlDialog��"
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
	/// bmp�Ƿ����
	/// </summary>
	bool isSnapShotExist(const std::string& path);
	/// <summary>
	/// path�Ƿ���Ч
	/// </summary>
	///<param name="timestamp">path��Ч,�򷵻�ʱ��</param >
	///<return>path�Ƿ���Ч</return>
	bool getPrjInfo(const std::string& path, std::string& timestamp,const char* surfix = "buildUp.bmp");
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
	DECLARE_DISPATCH_MAP()
public:
	/// <summary>
	/// ��������winXp��Ҳ��Ч,wow!
	/// </summary>
	HRESULT STDMETHODCALLTYPE ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved);
	/// <summary>
	/// �û����Ĳ˵�ѡ��
	/// </summary>
	/// <returns>���������ţ���pkpm.ini�µ�[Html]</returns>
	BSTR OnGetDefaultMenuSelection();
	/// <summary>
	/// ��ҳ��һ�λ��ᱣ���û����Ĳ˵�ѡ��
	/// </summary>
	/// <param name="selections">�˵�ѡ���ַ���</param>
	void OnSetDefaultMenuSelection(BSTR selections);
	//��ô�����������?
	void OnSetDefaultMenuSelection2(BSTR selections);
	//�õ��û���ǰѡ�������
	std::vector<std::pair<std::string, std::string>> MenuSelectionOnHtml();
	/// <summary>
	/// ǰ�˳���Ա�������ŵذ�һ���˵������˱��أ�ȴ�ֲ�����������˵�
	/// ���������μ��˵������ݴ��ݸ���ҳ��
	/// </summary>
	/// <param name="filename">�����ļ�·��</param>
	/// <returns>json��ʽ�������ļ�����</returns>
	BSTR OnGetConfig(BSTR);
	/// <summary>
	/// ��ҳ���"�½�"
	/// </summary>
	/// <returns>�û�ѡ���·��</returns>
	BSTR OnNewProject();
	/// <summary>
	/// ��ȡ������Ϣ
	/// </summary>
	/// <param name="fileName">�����ļ�cfg/pkpm.ini</param>
	/// <returns>����/ͼƬ·��������ʱ��</returns>
	BSTR ReadWorkPaths(BSTR fileName);
	/// <summary>
	/// ���ݸ�main�������ַ���
	/// </summary>
	/// <param name="fileName"></param>
	/// <param name="fileName"></param>
	void SetPkpmPara(BSTR path,BSTR core);
	/// <summary>
	/// ˫�����̵���Ӧ����
	/// </summary>
	/// <param name="prjPath">����·��</param>
	/// <param name="PathOfCore">exe·��</param>
	/// <param name="coreWithPara">����</param>
	void OnDbClickProject(BSTR prjPath, BSTR PathOfCore, BSTR coreWithPara, BSTR secondMenu, BSTR thirdMenu); //����������ҳ����
	//�б�˫��/����
	void OnListMenu(BSTR PathOfCore, BSTR coreWithPara, BSTR secondMenu, BSTR thirdMenu);

	void OnOpenDocument(BSTR RootInRibbon, BSTR filePath);
	/// <summary>
	/// ��ݼ���Ӧ����
	/// </summary>
	/// <param name="menuName">�˵���</param>
	void OnShortCut(const char* menuName);
	/// <summary>
	/// "����ת��"��Ӧ����
	/// </summary>
	/// <param name="prjName">�˵���</param>
	void DataFormatTransfer(BSTR);
	/// <summary>
	/// ���ӹ���
	/// </summary>
	/// <param name="newProj">�¹�����</param>
	/// <param name="filename">��д�������ļ�</param>
	/// <returns>�Ƿ���ӳɹ�</returns>
	bool AddWorkPaths(const std::string &newProj,const std::string& filename);
	/// <summary>
	/// ���湤��·���������ļ���֮ǰ��ȫ�ֺ���
	/// </summary>
	/// <param name="projs">������</param>
	/// <param name="filename">�����ļ�·��</param>
	void SaveWorkPaths(collection_utility::BoundedQueue<std::string>& projs, const std::string& filename);
	/// <summary>
	/// ��Ӧ�û����"��"
	/// </summary>
	/// <param name="prjName"></param>
	BSTR OnOpenProject();
	/// <summary>
	/// �Ҽ���������ͼ��
	/// </summary>
	/// <param name="prjName"></param>
	void OnRightClickProject(BSTR prjName);
	/// <summary>
	/// ������˫���ĸ�������
	/// </summary>
	/// <returns>�Ƿ�ȫ��</returns>
	///<remarks>should be private</remarks>
	bool IsFullScreen();
	/// <summary>
	/// ��ҳ���ñ��������ñ����л���õ�ie�汾
	/// </summary>
	void SetIeVersion(BSTR);
	/// <summary>
	/// ��ֹ��ҳ��ݼ�
	/// </summary>
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID);
	/// <summary>
	/// �ĵ�����֮�󣬲ſ�����com�������ҳ��Ԫ��
	/// </summary>
	virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	/// <summary>
	/// ûʱ��д
	/// </summary>
	///<remarks>should be private</remarks>
	bool CommandFliter(CString path,CString core, CString secMenu, CString trdMenu);
	void OnClickedResume();
	void SwitchSrcOfZoomAll(bool maxFlag);
	std::string GetIdByPos(const CPoint cp);
	bool VersionPage();
	BSTR TellMeNewVersionExistOrNot();
	BSTR TellMeAdvertisement();

	//��ݼ���Ӧ����
	void OnBnClickedRadSingle();
	void OnBnClickedRadNet();
	void OnDropFiles(HDROP hDropInfo);//Ϊ��IE���ݣ���Ҫ��ֹ��ק
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
	void OnBnClickedBtnFileMgr();//���ݴ��->"ģ�ʹ��"
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
	CString m_WorkPath;//һ��"�¾�"
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