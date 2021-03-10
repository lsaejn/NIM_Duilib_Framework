#pragma once

class VersionManager : public ui::WindowImplBase
{
public:
	VersionManager();
	~VersionManager();

	virtual std::wstring GetSkinFolder() override;
	virtual std::wstring GetSkinFile() override;
	virtual std::wstring GetWindowClassName() const override;
	virtual void InitWindow() override;
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	static const std::wstring kClassName;
private:
	void InitUiElement();
	void AttachClickToButton();
	void AttachClickToCheckBox();
	void InitList();

	ui::ListBox* list_ = nullptr;
	ui::Button* exitBtn_=nullptr;
	ui::Button* runBtn_ = nullptr;
	ui::Button* editBtn_ = nullptr;
	ui::Button* pathBtn_ = nullptr;
	ui::Button* createBtn_ = nullptr;
	ui::Button* deleteBtn_ = nullptr;
	ui::CheckBox* checkBox_ = nullptr;
};

