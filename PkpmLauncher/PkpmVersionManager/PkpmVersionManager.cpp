#include "stdafx.h"
#include "PkpmVersionManager.h"
#include "VersionItem.h"
#include "VersionEditPage.h"
#include "util.h"


const std::wstring VersionManager::kClassName = L"VersionManager";

VersionManager::VersionManager()
{
}


VersionManager::~VersionManager()
{
}

std::wstring VersionManager::GetSkinFolder()
{
	return L"manager";
}

std::wstring VersionManager::GetSkinFile()
{
	return L"manager.xml";
}

std::wstring VersionManager::GetWindowClassName() const
{
	return kClassName;
}

void VersionManager::InitWindow()
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
	InitUiElement();
	InitList();
	AttachClickToButton();
	AttachClickToCheckBox();

}

LRESULT VersionManager::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PostQuitMessage(0L);
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

void VersionManager::InitUiElement()
{
	exitBtn_ = dynamic_cast<ui::Button*>(FindControl(L"exit"));
	runBtn_ = dynamic_cast<ui::Button*>(FindControl(L"run"));
	editBtn_ = dynamic_cast<ui::Button*>(FindControl(L"version_edit"));
	pathBtn_ = dynamic_cast<ui::Button*>(FindControl(L"version_path"));
	deleteBtn_ = dynamic_cast<ui::Button*>(FindControl(L"version_delete"));
	createBtn_ = dynamic_cast<ui::Button*>(FindControl(L"version_creat"));
	checkBox_ = dynamic_cast<ui::CheckBox*>(FindControl(L"checker"));
	list_ = dynamic_cast<ui::ListBox*>(FindControl(L"list"));
}

void VersionManager::AttachClickToButton()
{
	exitBtn_->AttachClick([this](ui::EventArgs* args) {
		Close();
		return false;
		});

	runBtn_->AttachClick([this](ui::EventArgs* args) {
		//list_->sele
		//AppUtil::CreateProcessWithCommand();
		return false;
		});

	editBtn_->AttachClick([this](ui::EventArgs* args) {
		//fix me
		return false;
		});

	pathBtn_->AttachClick([this](ui::EventArgs* args) {
		//fix me
		return false;
		});

	deleteBtn_->AttachClick([this](ui::EventArgs* args) {
		//fix me
		auto index=list_->GetCurSel();
		if (index == -1)
		{
			//fix me
			
		}
		else
		{
			//auto item = dynamic_cast<Item*>(list_->GetItemAt(index));
			//assert(item);
			list_->RemoveAt(index);
		}
		return true;
		});

	createBtn_->AttachClick([this](ui::EventArgs* args) {
		//fix me
		Alime::ShowMsgBox(GetHWND(), [this](Alime::MsgBoxRet ret) {
			std::wstring ver = L"newV";
			std::wstring path = L"newP";
			Item* item = new Item;
			ui::GlobalManager::FillBoxWithCache(item, L"manager/item.xml");

			item->InitSubControls(ver, path);
			list_->AddAt(item, 0);
			list_->SelectItem(0);
			return;
			});
		return true;
	});


}

void VersionManager::AttachClickToCheckBox()
{
	auto func = [this](ui::EventArgs* args) {
		if (checkBox_->IsSelected())
		{
			OutputDebugString(L"user selected checkBox");
		}
		else {
			OutputDebugString(L"user unSelected checkBox");
		}
		return false;
	};
	checkBox_->AttachUnSelect(func);
	checkBox_->AttachSelect(func);
}

void VersionManager::InitList()
{
	for (auto i = 0; i < 10; i++)
	{
		Item* item = new Item;
		ui::GlobalManager::FillBoxWithCache(item, L"manager/item.xml");

		std::wstring userDefineName = L"PkpmV52-"+ std::to_wstring(i);
		std::wstring cfgPath = nbase::StringPrintf(L"下载任务 [%02d]", i + 1);

		item->InitSubControls(userDefineName, cfgPath);
		list_->Add(item);
	}

	// 监听列表中点击选择子项的事件
	list_->AttachSelect([this](ui::EventArgs* args) {
		int current = args->wParam;
		int old = args->lParam;
		auto message = nbase::StringPrintf(L"您选择了索引为 %d 的子项，上一次选择子项索引为 %d", current, old);
		//nim_comp::ShowMsgBox(GetHWND(), nim_comp::MsgboxCallback(), message, false, L"提示", false);
		return false;
		});


}
