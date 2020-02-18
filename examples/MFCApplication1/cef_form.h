#pragma once

#include "WebDataReader.h"
#include "BoundedQueue.h"

#define WEBINTERFACE

// �����Ƿ�ʹ��������Ⱦ����Ϊ true  ʱ��ʹ��Ĭ�ϴ�����Ӱ������������Ⱦģʽ�´����� WS_EX_LAYERED ����
// ��Ϊ false ʱ��ʹ�����洰��ģʽ��֧�ִ��� WS_EX_LAYERED ���Դ��ڣ�����ʹ�����ô�����Ӱ��������Ҫ�� xml �н����� shadowattached ��������Ϊ false
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

	

	
	//ǰ�˶�ȡ�˵�
	WEBINTERFACE std::string ReadWorkPathFromFile(const std::string& iniFileName);
	//��ȡ������Ϣ
	WEBINTERFACE bool GetPrjInfo(const std::string& path, std::string& timestamp, const char* surfix = "buildUp.bmp");
	//�޸ı�����
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

