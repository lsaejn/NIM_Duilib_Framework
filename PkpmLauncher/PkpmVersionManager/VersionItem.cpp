#include "stdafx.h"
#include "VersionItem.h"


Item::Item()
{
}


Item::~Item()
{
}

void Item::InitSubControls(const std::wstring& title, const std::wstring& cfgPath)
{
	// ���� Item �µĿؼ�
	control_img_	= dynamic_cast<ui::Label*>(FindSubControl(L"versionName"));
	label_title_	= dynamic_cast<ui::Label*>(FindSubControl(L"label_title"));

	// ģ�����������
	//nbase::TimeDelta time_delta = nbase::TimeDelta::FromMicroseconds(nbase::Time::Now().ToInternalValue());
	//progress_->SetValue((double)(time_delta.ToMilliseconds() % 100));

	// ����ͼ�����������
	control_img_->SetText(title);
	//label_title_->SetText(nbase::StringPrintf(L"%s %d%%", title.c_str(), time_delta.ToMilliseconds() % 100));

	// ��ɾ����������
	//btn_del_->AttachClick(nbase::Bind(&Item::OnRemove, this, std::placeholders::_1));
}

bool Item::OnRemove(ui::EventArgs* args)
{
	ui::ListBox* parent = dynamic_cast<ui::ListBox*>(this->GetParent());
	return parent->Remove(this);
}
