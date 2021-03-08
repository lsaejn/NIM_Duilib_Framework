#include "stdafx.h"
#include "PkpmVersionManager.h"
#include "VersionItem.h"


const std::wstring BasicForm::kClassName = L"Basic";

BasicForm::BasicForm()
{
}


BasicForm::~BasicForm()
{
}

std::wstring BasicForm::GetSkinFolder()
{
	return L"manager";
}

std::wstring BasicForm::GetSkinFile()
{
	return L"manager.xml";
}

std::wstring BasicForm::GetWindowClassName() const
{
	return kClassName;
}

void BasicForm::InitWindow()
{
	//ui::ListBox* list = static_cast<ui::ListBox*>(FindControl(L"list"));
	//for (auto i = 0; i < 30; i++)
	//{
	//	ui::ListContainerElement* element = new ui::ListContainerElement;
	//	element->SetText(nbase::StringPrintf(L"ListElement %d", i));
	//	element->SetClass(L"listitem");
	//	element->SetFixedHeight(20);
	//	list->Add(element);
	//}

	ui::ListBox*  list_ = dynamic_cast<ui::ListBox*>(FindControl(L"list"));

	for (auto i = 0; i < 100; i++)
	{
		Item* item = new Item;
		ui::GlobalManager::FillBoxWithCache(item , L"manager/item.xml");

		std::wstring img = L"icon.png";
		std::wstring title = nbase::StringPrintf(L"下载任务 [%02d]", i + 1);

		item->InitSubControls(img, title);
		list_->Add(item);
	}

	// 监听列表中点击选择子项的事件
	list_->AttachSelect([this](ui::EventArgs* args) {
		//fix me
		return false;
		});
}

LRESULT BasicForm::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PostQuitMessage(0L);
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}
