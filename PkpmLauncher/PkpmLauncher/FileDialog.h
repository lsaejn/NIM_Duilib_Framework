#pragma once 

#include <string>
#include "base/base.h"

#include "afxdlgs.h"
#include "Shobjidl.h"
#include "windows.h"

#define implements :public

class IFolderDialog
{
public:
	~IFolderDialog()=default;
	//virtual std::list<std::string> GetOpenFilePaths() {};
	//virtual std::string GetOpenFilePath() {};
	virtual std::string GetExistingDirectory(const char* caption=NULL,
		const char* dir=NULL,
		bool showDirsOnly=true
		)=0;
};

typedef void (*func)(const char*, char*);

class OldStyleFileDialog implements IFolderDialog
{
public:
	virtual std::string GetExistingDirectory(const char* /*caption*/,
		const char* dir, bool /*showDirsOnly*/)
	{
		auto path=nbase::win32::GetCurrentModuleDirectory();	
		auto hdll = LoadLibrary((path+L"PKPM2010V511.dll").c_str());
		if(!hdll)	return {};
		func ptr = (func)GetProcAddress(hdll, "OpenDlgSelectDir");
		if (!ptr)	return {};
		char result[MAX_PATH] = { 0 };
		std::string defaultPath;
		if (dir)
			defaultPath = dir;
		ptr(defaultPath.c_str(), result);
		FreeLibrary(hdll);
		return result;
	}
};

class NativeFileDialog implements IFolderDialog
{
public:
	virtual std::string GetExistingDirectory(const char* /*caption*/,
		const char* /*dir*/, bool /*showDirsOnly*/)
	{
		IFileDialog* pfd = NULL;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pfd));

		DWORD dwFlags;;
		hr = pfd->GetOptions(&dwFlags);
		hr = pfd->SetOptions(dwFlags | FOS_PICKFOLDERS);
		hr = pfd->Show(NULL);
		IShellItem* psiResult;
		hr = pfd->GetResult(&psiResult);
		PWSTR pszFilePath = NULL;
		hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH,
			&pszFilePath);
		std::string result = nbase::UTF16ToUTF8(pszFilePath);
		CoTaskMemFree(pszFilePath);
		pfd->Release();
		return result;
	}
};

class QtFileDialog implements IFolderDialog
{
public:
	virtual std::string GetExistingDirectory(const char* /*caption*/,
		const char* /*dir*/, bool /*showDirsOnly*/)
	{
		auto path = nbase::win32::GetCurrentModuleDirectory();

		HMODULE hDll = NULL;
		hDll=LoadLibrary((path + L"resources\\QtFolderDialog.dll").c_str());
		if (hDll)
		{
			typedef bool(*pShow)(HWND parent);
			pShow fp1 = pShow(GetProcAddress(hDll, "showFolderDialog"));
			if (fp1)
				fp1(NULL);
			FreeLibrary(hDll);
		}
		else
		{
			OutputDebugString(L"Fail to Open QtDialog");
		}
		return {};
	}
};

class ShBrowserFileDialog implements IFolderDialog
{
public:
	virtual std::string GetExistingDirectory(const char* /*caption*/,
		const char* /*dir*/, bool /*showDirsOnly*/)
	{
		TCHAR  folderPath[MAX_PATH] = { 0 };
		std::wstring path;

		BROWSEINFO  sInfo;
		::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
		sInfo.pidlRoot = 0;
		sInfo.lpszTitle = _T("请选择新的工程路径");
		sInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX
			| BIF_DONTGOBELOWDOMAIN | BIF_USENEWUI;
		sInfo.lpfn = NULL;

		LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);
		if (lpidlBrowse != NULL)
		{
			if (::SHGetPathFromIDList(lpidlBrowse, folderPath))
			{
				path = folderPath;
				::CoTaskMemFree(lpidlBrowse);
				return nbase::UnicodeToAnsi(path);
			}
		}
		return {};
	}
};

class FolderDialogFactory
{
public:
	enum class FdType
	{
		OLD_STYLE,
		NATIVE,
		QT,
		SHBROWSER
	};
	static FdType GetFdType(size_t index)
	{
		if (index >= sizeof FdType)
			throw std::invalid_argument("bad index");
		FdType ty=static_cast<FdType>(index);
		return ty;
	}

	static std::shared_ptr<IFolderDialog> GetFolderDialog(FdType ty)
	{
		if (ty == FdType::NATIVE)
			return std::make_shared<NativeFileDialog>();
		if (ty == FdType::QT)
			return std::make_shared<QtFileDialog>();
		if (ty == FdType::SHBROWSER)
			return  std::make_shared<ShBrowserFileDialog>();
		return 
			std::make_shared<OldStyleFileDialog>();
	}
};