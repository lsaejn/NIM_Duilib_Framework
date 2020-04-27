#include "pch.h"
#include "MessageLoop.h"
#include "ConfigFileManager.h"

void MainThread::Init()
{
	nbase::ThreadManager::RegisterThread(0);
	// ��ȡ��Դ·������ʼ��ȫ�ֲ���
	// Ĭ��Ƥ��ʹ�� resources\\themes\\default
	// Ĭ������ʹ�� resources\\lang\\zh_CN
	// �����޸���ָ�� Startup �����������
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
	//����ʹ�����繦��
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