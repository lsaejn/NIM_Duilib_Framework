#include "pch.h"
#include "MessageLoop.h"
#include "ConfigFileManager.h"

void MainThread::Init()
{
	nbase::ThreadManager::RegisterThread(0);
	// 获取资源路径，初始化全局参数
	// 默认皮肤使用 resources\\themes\\default
	// 默认语言使用 resources\\lang\\zh_CN
	// 如需修改请指定 Startup 最后两个参数
	std::wstring theme_dir = nbase::win32::GetCurrentModuleDirectory();
	ui::GlobalManager::Startup(theme_dir + L"resources\\", ui::CreateControlCallback(), isAdaptDpiOpen());
	SpecificInit();
}

void MainThread::Cleanup()
{
	ui::GlobalManager::Shutdown();
	SetThreadWasQuitProperly(true);
	nbase::ThreadManager::UnregisterThread();
}

void MainThread::SpecificInit()
{
	//比如使用网络功能
	f_();
}

void MainThread::setFunc(const Func& func)
{
	f_ = func;
}

bool MainThread::isAdaptDpiOpen()
{
	auto& configfile = ConfigManager::GetInstance();
	return configfile.IsAdaptDpiOn();
}