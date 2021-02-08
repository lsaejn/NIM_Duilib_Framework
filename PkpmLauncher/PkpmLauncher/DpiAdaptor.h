#pragma once
#include "Alime/NonCopyable.h"
#include "ConfigFileManager.h"
#include <memory>

/// <summary>
/// ������Ӧdpi�ķ�����1�ǿ���Դ���������Ҫ�Լ�����ͼƬ
/// 2��׼�����xml������ʹ�õ��Ǻ��ߡ�
/// �����������Ҫ�Լ�����������϶��߶Ⱥ�cef���ڷŴ�ϵ��
/// DPI���������������ף�����ͼƬ��Դ������ʾ��ݡ�
/// </summary>

class CefForm;

class IAdaptor
{
public:
	virtual void AdaptCaption(CefForm*)=0;
	virtual void AdaptCefWindow(CefForm*)=0;
	virtual std::wstring SelectSkinFile();
	UINT GetDpi();
	double GetScale();
};

class DefaultDpiAdaptor:public IAdaptor
{
public:
	DefaultDpiAdaptor() = default;

	virtual void AdaptCaption(CefForm* _window)  override;
	virtual void AdaptCefWindow(CefForm* _window) override;
	double GetZoomLevel();
	std::wstring SelectSkinFile();
private:
	std::wstring SkinFileAsDpi(const std::wstring& file, const std::wstring& suffix);
};

class EmptyDpiAdaptor :public IAdaptor
{
public:
	EmptyDpiAdaptor() = default;

	virtual void AdaptCaption(CefForm* /*_window*/) {};
	virtual void AdaptCefWindow(CefForm* /*_window*/) {};
};

class DpiAdaptorFactory:public noncopyable
{
public:
	enum class AdaptorTy
	{
		Default,
		CaptionInclude
	};

	static std::shared_ptr<IAdaptor> GetAdaptor()
	{
		if(ConfigManager::GetInstance().IsAdaptDpiOn() ||
			ConfigManager::GetInstance().IsModifyWindowOn()
			)
			return std::make_shared<DefaultDpiAdaptor>();
		else
			return std::make_shared<EmptyDpiAdaptor>();
	}

private:
	DpiAdaptorFactory() = default;
	~DpiAdaptorFactory()=default;
};

