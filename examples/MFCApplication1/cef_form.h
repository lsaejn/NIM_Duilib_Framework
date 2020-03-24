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
	
	/// <summary>������Դ�ļ���</summary>
	virtual std::wstring GetSkinFolder() override;

	/// <summary>���ý����ļ�</summary>
	virtual std::wstring GetSkinFile() override;

	/// <summary>��������</summary>
	virtual std::wstring GetWindowClassName() const override;

	/// <summary>cefcontrol��xml���Զ�������,�Լ���������</summary>
	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;
	virtual void InitWindow() override;
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	static const std::wstring	kClassName;

private:
	/// <summary>�ؼ�����¼�,xml�����һЩ�ؼ��������Ժܷ���</summary>
	bool OnClicked(ui::EventArgs* msg);
	bool OnNavigate(ui::EventArgs* msg);
	void OnLoadEnd(int httpStatusCode);
	bool OnDbClicked(ui::EventArgs* msg);

	/// <summary>ǰ�˶�ȡ�����б�,�ɴ���,��ע��</summary>
	///<param name="iniFileName">�����ļ�����ļ�·��</param>
	///<return>�����б��json������+����ʱ��+����ͼ·��</return>
	WEBINTERFACE std::string ReadWorkPathFromFile(const std::string& iniFilePath);

	/// <summary>����������c++�ڴ���ǰ�˵��������Ҫ���ô˺���</summary>
	///<param name="name">����������</param >
	WEBINTERFACE void SetCaption(const std::string& name);
	
	/// <summary>�˵����书�ܣ�ǰ�˵��ô˺�������ļ��ﱣ�������</summary>
	///<return>4��������json</return>
	WEBINTERFACE std::string OnGetDefaultMenuSelection();
	
	/// <summary>��ҳ����¼�</summary>
	///<param name="name">��ҳ�Ͽ�ݼ����ַ���</param >
	WEBINTERFACE void OnShortCut(const char* cutName);
	
	/// <summary>�½�����</summary>
	///<return>�û�ѡ��Ĺ���·��</return>
	WEBINTERFACE std::string OnNewProject();

	/// <summary>��ҳ��࣬����ת�����õĺ���������ʷԭ������ڵĵ����ӿڣ���������׼ȷ</summary>
	///<param name="module">ģ��,�����������"����ת��"</param >
	///<param name="InterfaceName">�ӿ���</param >
	///<param name="workdir">����Ŀ¼</param >
	WEBINTERFACE void DataFormatTransfer(const std::string& module, const std::string& InterfaceName, const std::string& workdir);

	/// <summary>���빤�̵ĺ���������Ҫ�ĺ���</summary>
	///<param name="args">��Ϊ����������Ǵ���ŵ�main������ϴ�����ʹ����5������</param >
	WEBINTERFACE void OnDbClickProject(const std::vector<std::string>& args);

	/// <summary>ĳЩ�����˵�˫����Ҫ���õĺ�����ֱ��ת����OnDbClickProject</summary>
	///<param name="args">ͬOnDbClickProject</param >
	WEBINTERFACE void OnListMenu(const std::vector<std::string>& args);

	/// <summary>�򿪱����ļ��Ľӿ�</summary>
	///<param name="filePath">�ļ�����ribbonΪ��Ŀ¼��������ʷԭ��</param >
	WEBINTERFACE void OnOpenDocument(const std::string& filePath);

	/// <summary>��ӹ��̵��ļ�</summary>
	///<param name="newProj">�¹�����</param >
	///<param name="filePath">�����ļ���</param >
	///<return>�����Ƿ����ӳɹ�</return>
	WEBINTERFACE bool AddWorkPaths(const std::string& newProj, const std::string& filename);

	/// <summary>���湤�̵��ļ�,�ɴ�������static��������Ҳ�����ټ�һ�������ˡ�</summary>
	///<param name="prjPaths">�����б�prjPathsԭ�����ǳ�Ա����</param >
	///<param name="filePath">��ʷԭ�������ļ�·��</param >
	WEBINTERFACE void SaveWorkPaths(collection_utility::BoundedQueue<std::string>& prjPaths, const std::string& filePath);

	/// <summary>����˵�����������ֻ�йر�ʱ�ű����ļ���</summary>
	///<param name="filePath">json, һ�������˵�����+������������</param >
	WEBINTERFACE void OnSetDefaultMenuSelection(const std::string& json_str);

	/// <summary>��֪ǰ�˹�����ݡ�</summary>
	///<return>������ݵ�json�����ӿ��ĵ�</return>
	WEBINTERFACE std::string TellMeAdvertisement();

	/// <summary>��֪ǰ���Ƿ�����°汾��</summary>
	///<return>bool</return>
	WEBINTERFACE bool TellMeNewVersionExistOrNot();

	/// <summary>�л�������Ϊindex������</summary>
	///<param name="index">����</param >
	void SwicthThemeTo(int index);

	/// <summary>�����û��Ļ���ѡ��</summary>
	///<param name="index">����</param >
	void SaveThemeIndex(int index);

	/// <summary>�Ҽ�����</summary>
	///<param name="prjPath">����·��</param >
	void OnRightClickProject(const std::wstring& prjPath);

	/// <summary>�������µĳ���汾����ʷԭ��cfgĿ¼�´������� v4.3.4.ini ֮����ı������ֳ���ǰ�汾</summary>
	///<param name="filenames">�ļ�������</param >
	///<param name="major_version">���汾��</param >
	///<param name="minor_version">���汾��</param >
	///<param name="sub_version">�Ӱ汾��</param >
	void getLatestVersion(std::vector<std::string>& filenames, int& major_version, int& minor_version, int& sub_version);

	/// <summary>Ѱ��ָ����ʽ���ļ���</summary>
	///<param name="path">��Ѱ�ҵ�Ŀ¼</param >
	///<param name="prefix">ǰ׺</param >
	///<param name="suffix">��׺</param >
	std::vector<std::string> CefForm::FindVersionFiles(const char* path, const char* prefix, const char* suffix);

	/// <summary>�����ʵ·������Ϊ����c++17�ĺ������Ҳ�ȷ���Ƿ�������</summary>
	///<param name="path">ԭ·��</param >
	///<param name="prefix">�����Ƿ�Ϸ�</param >
	///<return>��ʵ·��</return>
	std::string PathChecker(const std::string& path, bool& legal);

	/// <summary>���̻߳�ȡ���ҳ��</summary>
	void InitAdvertisement();
	/// <summary>��ȡ���ҳ����̺߳���</summary>
	void AdvertisementThreadFunc();

	/// <summary>�ɴ��룬�������ˡ����ع��ҳ��</summary>
	bool VersionPage();

	/// <summary>�ɴ��룬�������ˡ���ù��̵Ĵ���ʱ�䣬���ؿ����Ƿ����</summary>
	///<param name="timestamp">�����ص�ʱ���</param >
	///<param name="surfix">���������շŵ��˱�ĺ���</param >
	///<return>Ҫ��ѯ���ļ��Ƿ���Ч< / return>
	bool GetPrjInfo(const std::string& path, std::string& timestamp, const char* surfix = "buildUp.bmp");

	/// <summary>ע��c++������js</summary>
	void RegisterCppFuncs();

	/// <summary>�Ƿ����buildup.bmp</summary>
	bool IsSnapShotExist(const std::string& path);

	/// <summary>�Ϻ�����ִ��dll��ĺ���ִ������</summary>
	void run_cmd(const CStringA& moduleName, const CStringA& appName1, const CStringA& appName2);

	/// <summary>�Ϻ���������PM��������</summary>
	bool SetCfgPmEnv();

	/// <summary>�û�ɾ�������ļ��У�Ȼ���½���Сд������ļ����ٴ򿪳�����͵��������ļ���Ĺ���·����һ���Ǵ�Сд���е�·��</summary>
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
	std::mutex lock_;//fix me�ĳ�ԭ�ӱ�����������ȥ����
	std::atomic<bool> isWebPageAvailable_;
	std::string pageInfo_;
	std::wstring defaultCaption_;
	int indexHeightLighted_;//�������첽ͨ�ţ����ܶ�ɴ�����Ҫֱ���ü������������������Ҫ�����²ߡ�
};

