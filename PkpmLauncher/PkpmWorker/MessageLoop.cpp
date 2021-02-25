#include "pch.h"
#include "MessageLoop.h"
#include "ConfigFileManager.h"

void MainThread::Init()
{
	nbase::ThreadManager::RegisterThread(0);
	std::wstring theme_dir = nbase::win32::GetCurrentModuleDirectory();
	ui::GlobalManager::Startup(theme_dir + L"resources\\", 
		ui::CreateControlCallback(),
		isAdaptDpiOpen(),
		L"themes\\default",
		ConfigManager::GetInstance().GetLanguageFile()
	);
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