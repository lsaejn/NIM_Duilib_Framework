#pragma once
#include "duilib/UIlib.h"

namespace nim_comp {

	typedef std::function<void(MsgBoxRet)> MsgboxCallback;

	class CircleBox;
	CircleBox* ShowCircleBox(HWND hwnd, MsgboxCallback cb, const std::wstring& title, bool title_is_id = false);


	/// <summary>
	/// 一个异步进度条
	/// </summary>
	class CircleBox : public ui::WindowImplBase
	{
	public:
		CircleBox();
		virtual ~CircleBox();

		virtual std::wstring GetSkinFolder() override;
		virtual std::wstring GetSkinFile() override;

		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		virtual void OnEsc(BOOL& bHandled);
		virtual void Close(UINT nRet = IDOK) override;

		virtual std::wstring GetWindowClassName() const override;
		virtual std::wstring GetWindowId() const /*override*/;
		virtual UINT GetClassStyle() const override;
		virtual void InitWindow() override;
	public:
		bool OnClicked(ui::EventArgs* msg);

		void SetTitle(const std::wstring& str);
		void Show(HWND hwnd, MsgboxCallback cb);

		void EndMsgBox(MsgBoxRet ret);
	public:
		static const LPCTSTR kClassName;
	private:
		ui::Label* title_;

		MsgboxCallback	 msgbox_callback_;
	};

}

