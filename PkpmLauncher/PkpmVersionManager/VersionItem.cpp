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
	// 查找 Item 下的控件
	control_img_	= dynamic_cast<ui::Label*>(FindSubControl(L"versionName"));
	label_title_	= dynamic_cast<ui::Label*>(FindSubControl(L"label_title"));

	// 模拟进度条进度
	//nbase::TimeDelta time_delta = nbase::TimeDelta::FromMicroseconds(nbase::Time::Now().ToInternalValue());
	//progress_->SetValue((double)(time_delta.ToMilliseconds() % 100));

	// 设置图标和任务名称
	control_img_->SetText(title);
	//label_title_->SetText(nbase::StringPrintf(L"%s %d%%", title.c_str(), time_delta.ToMilliseconds() % 100));

	// 绑定删除任务处理函数
	//btn_del_->AttachClick(nbase::Bind(&Item::OnRemove, this, std::placeholders::_1));
}

bool Item::OnRemove(ui::EventArgs* args)
{
	ui::ListBox* parent = dynamic_cast<ui::ListBox*>(this->GetParent());
	return parent->Remove(this);
}
