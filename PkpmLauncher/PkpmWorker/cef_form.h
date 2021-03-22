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
	
	/// <summary>������Դ�ļ���</summary>
	virtual std::wstring GetSkinFolder() override;

	/// <summary>���ý����ļ�</summary>
	virtual std::wstring GetSkinFile() override;

	/// <summary>��������</summary>
	virtual std::wstring GetWindowClassName() const override;

	/// <summary>cefcontrol��xml���Զ�������,�Լ���������</summary>
	virtual ui::Control* CreateControl(const std::wstring& pstrClass) override;
	virtual void InitWindow() override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual LRESULT OnNcLButtonDbClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	
private:

	/// <summary>�ؼ�����¼�,xml�����һЩ�ؼ��������Ժܷ���</summary>
	bool OnClicked(ui::EventArgs* msg);
	void OnLoadEnd(int httpStatusCode);
	void InitUiVariable();
	void AttachFunctionToShortCut();
	void AppendThreadTask();
	/// <summary>ǰ�˶�ȡ�����б�,�ɴ���,��ע��</summary>
	///<param name="iniFileName">�����ļ�����ļ�·��</param>
	///<return>�����б��json������+����ʱ��+����ͼ·��</return>
	std::string ReadWorkPathFromFile(const std::string& iniFilePath);

	/// <summary>����������c++�ڴ���ǰ�˵��������Ҫ���ô˺���</summary>
	///<param name="name">����������</param >
	void SetCaptionWithProjectName(const std::string& prjname);
	
	/// <summary>�˵����书�ܣ�ǰ�˵��ô˺�������ļ��ﱣ�������</summary>
	///<return>4��������json</return>
	std::string OnGetDefaultMenuSelection();
	
	/// <summary>��ҳ����¼�</summary>
	///<param name="name">��ҳ�Ͽ�ݼ����ַ���</param >
	void OnShortCut(const std::wstring& cutName);
	
	/// <summary>�½�����</summary>
	///<return>�û�ѡ��Ĺ���·��</return>
	std::string OnNewProject();

	/// <summary>����ת�����°汾���ѷ������������˽ӿڣ������ó������¼���(ǰ����ʹ���������ļ�)</summary>
	///<param name="module">ģ��,�����������"����ת��"</param >
	///<param name="InterfaceName">�ӿ���</param >
	///<param name="workdir">����Ŀ¼</param >
	void DataFormatTransfer(const std::string& module, const std::string& InterfaceName, const std::string& workdir);

	/// <summary>���빤�̵ĺ���������Ҫ�ĺ���</summary>
	///<param name="args">��Ϊ����������Ǵ���ŵ�main������ϴ�����ʹ����5������</param >
	void OnDbClickProject(const std::vector<std::string>& args);

	/// <summary>ĳЩ�����˵�˫����Ҫ���õĺ�����ֱ��ת����OnDbClickProject</summary>
	///<param name="args">ͬOnDbClickProject</param >
	void OnListMenu(const std::vector<std::string>& args);

	/// <summary>�򿪱����ļ��Ľӿ�, ���û��ֲ�͸���˵��</summary>
	///<param name="filePath">�ļ�����ribbonΪ��Ŀ¼��������ʷԭ��</param >
	void OnOpenDocument(const std::string& filePath);

	/// <summary>��ӹ��̵��ļ�</summary>
	///<param name="newProj">�¹�����</param >
	///<param name="filePath">�����ļ���</param >
	///<return>�����Ƿ����ӳɹ�</return>
	bool AddWorkPaths(const std::string& newProj, const std::string& filename);

	/// <summary>���湤�̵��ļ�,�ɴ�������static��������Ҳ�����ټ�һ�������ˡ�</summary>
	///<param name="prjPaths">�����б�prjPathsԭ�����ǳ�Ա����</param >
	///<param name="filePath">��ʷԭ�������ļ�·��</param >
	void SaveWorkPaths(collection_utility::BoundedQueue<std::string>& prjPaths, const std::string& filePath);

	/// <summary>����˵�����������ֻ�йر�ʱ�ű����ļ���</summary>
	///<param name="filePath">json, һ�������˵�����+������������</param >
	void OnSetDefaultMenuSelection(const std::string& json_str);

	/// <summary>��֪ǰ�˹�����ݡ�</summary>
	///<return>������ݵ�u8json�����ӿ��ĵ�</return>
	std::string TellMeAdvertisement();

	/// <summary>
	/// ��֪ǰ���Ƿ�����°汾���²���������������
	/// �в������и��°�����ô����û��Ҫ��ⲹ����
	/// </summary>
	///<return>bool</return>
	bool TellMeNewVersionExistOrNot();

	/// <summary>�������л�������Ϊindex������</summary>
	///<param name="index">����</param >
	///<return>�Ƿ�ɹ�</return>
	bool SwicthThemeTo(int index);

	/// <summary>�����û��Ļ���ѡ��</summary>
	///<param name="index">����</param >
	/// <param name="name">����</param >
	void SaveThemeIndex(int index, const std::wstring& name);

	/// <summary>�Ҽ�����</summary>
	///<param name="prjPath">����·��</param >
	void OnRightClickProject(const std::wstring& prjPath);

	/// <summary>��ȡ��Сд���е�·��</summary>
	///<param name="path">ԭ·��</param >
	///<param name="prefix">�����Ƿ�Ϸ�</param >
	///<return>��ʵ·��</return>
	std::string PathChecker(const std::string& path, bool& legal);

	/// <summary>�ɴ��룬�������ˡ���ù��̵Ĵ���ʱ�䣬���ؿ����Ƿ����</summary>
	///<param name="timestamp">�����ص�ʱ���</param >
	///<param name="surfix">���������շŵ��˱�ĺ���</param >
	///<return>Ҫ��ѯ�Ĺ����Ƿ���Ч< / return>
	bool GetPrjInfo(const std::string& path, std::string& timestamp, const char* surfix = "buildUp.bmp");

	/// <summary>ע��c++������js</summary>
	void RegisterCppFuncs();

	///<summary>����ʱ���õĺ���</summary>
	void OnLoadStart();

	/// <summary>�Ƿ����buildup.bmp</summary>
	///<param name="path">����·��</param >
	bool IsSnapShotExist(const std::string& path);

	/// <summary>�Ϻ�����ִ��dll��ĺ���ִ��������ڵĽ���</summary>
	///<param name="moduleName">�����˵�</param >
	///<param name="appName1">�����˵�</param >
	///<param name="appName2">������û��</param >
	void run_cmd(const std::string& moduleName, const std::string& appName1, const std::string& appName2);

	/// <summary>�Ϻ��������û�������</summary>
	bool SetEnv();

	/// <summary>�û�ɾ�������ļ��У�Ȼ���½���Сд������ļ����ٴ򿪳�����͵��������ļ���Ĺ���·����һ���Ǵ�Сд���е�·��</summary>
	///<return>��Ч�Ĺ�������</return>
	size_t CorrectWorkPath();

	/// <summary>���õ�ǰ����������</summary>
	///<param name="index">�������</param >
	void SetHeightLightIndex(const int index);

	/// <summary>�Ի�������ļ���ק</summary>
	void EnableAcceptFiles();

	/// <summary>����,������ť���¼�</summary>
	void AttachClickCallbackToSkinButton();

	/// <summary>��ʼ����־��</summary>
	void InitSpdLog();

	/// <summary>�򿪲���װ��ʽ</summary>
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

