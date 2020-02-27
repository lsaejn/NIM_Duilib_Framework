#include "StdAfx.h"
#include "CommonUtil.h"
#include <io.h>
#include <memory>
#include <functional>
#include <sys/stat.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL sjx::IsFolderExist(CString csFolderPath)
{
    struct _stat64i32 s;
    if (_tstat(csFolderPath, &s) == 0)
    {
        if (s.st_mode & S_IFDIR)
        {
            return TRUE;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

BOOL sjx::IsFileExist(CString csFilePath)
{
    struct _stat64i32 s;
    if (_tstat(csFilePath, &s) == 0)
    {
        if (s.st_mode & S_IFREG)
        {
            return TRUE;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

CString sjx::GetModuleFolder()
{
    TCHAR FilePath[MAX_PATH + 1] = { 0 };
    GetModuleFileName(NULL, FilePath, sizeof(FilePath)); //获取程序当前执行文件名
    CString csFilePath(FilePath);

    int index = csFilePath.ReverseFind(_T('\\'));
    return csFilePath.Left(index + 1);
}
