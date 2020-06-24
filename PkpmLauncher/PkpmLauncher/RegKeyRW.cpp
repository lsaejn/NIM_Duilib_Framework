#include "pch.h"
#include "windows.h"
#include "RegKeyRW.h"
const int buffSize = 256;

/*
刘超提供的代码，这么恶心的实现，我也是醉了
*/

std::string ws2s(const std::wstring& ws)
{
    std::string curLocale = setlocale(LC_ALL, "");
    const wchar_t* _Source = ws.c_str();
    size_t _Dsize = wcstombs(NULL, _Source, 0) + 1;
    char* _Dest = new char[_Dsize];
    memset(_Dest, 0, _Dsize);
    wcstombs(_Dest, _Source, _Dsize);
    std::string result = _Dest;
    delete[]_Dest;
    setlocale(LC_ALL, curLocale.c_str());
    return result;
}

std::wstring s2ws(const std::string& s)
{
    std::string curLocale = setlocale(LC_ALL, "");
    const char* _Source = s.c_str();
    size_t _Dsize = mbstowcs(NULL, _Source, 0) + 1;
    wchar_t* _Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
    mbstowcs(_Dest, _Source, _Dsize);
    std::wstring result = _Dest;
    delete[]_Dest;
    setlocale(LC_ALL, curLocale.c_str());
    return result;
}

std::string GetRegValue(int nKeyType, const std::string& strUrl, const std::string& strKey)
{
    std::string strValue("");
    HKEY hKey = NULL;
    HKEY  hKeyResult = NULL;
    DWORD dwSize = 0;
    DWORD dwDataType = 0;
    std::wstring wstrUrl = s2ws(strUrl);
    std::wstring wstrKey = s2ws(strKey);

    switch (nKeyType)
    {
    case 0:
    {
        hKey = HKEY_CLASSES_ROOT;
        break;
    }
    case 1:
    {
        hKey = HKEY_CURRENT_USER;
        break;
    }
    case 2:
    {
        hKey = HKEY_LOCAL_MACHINE;
        break;
    }
    case 3:
    {
        hKey = HKEY_USERS;
        break;
    }
    case 4:
    {
        hKey = HKEY_PERFORMANCE_DATA;
        break;
    }
    case 5:
    {
        hKey = HKEY_CURRENT_CONFIG;
        break;
    }
    case 6:
    {
        hKey = HKEY_DYN_DATA;
        break;
    }
    case 7:
    {
        hKey = HKEY_CURRENT_USER_LOCAL_SETTINGS;
        break;
    }
    case 8:
    {
        hKey = HKEY_PERFORMANCE_TEXT;
        break;
    }
    case 9:
    {
        hKey = HKEY_PERFORMANCE_NLSTEXT;
        break;
    }
    default:
    {
        return strValue;
    }
    }

    //打开注册表
    if (ERROR_SUCCESS == ::RegOpenKeyEx(hKey, wstrUrl.c_str(), 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hKeyResult))
    {
        // 获取缓存的长度dwSize及类型dwDataType
        ::RegQueryValueEx(hKeyResult, wstrKey.c_str(), 0, &dwDataType, NULL, &dwSize);
        switch (dwDataType)
        {
        case REG_MULTI_SZ:
        {
            //分配内存大小
            BYTE* lpValue = new BYTE[dwSize];
            //获取注册表中指定的键所对应的值
            LONG lRet = ::RegQueryValueEx(hKeyResult, wstrKey.c_str(), 0, &dwDataType, lpValue, &dwSize);
            delete[] lpValue;
            break;
        }
        case REG_SZ:
        {
            //分配内存大小
            wchar_t* lpValue = new wchar_t[dwSize];
            memset(lpValue, 0, dwSize * sizeof(wchar_t));
            //获取注册表中指定的键所对应的值
            if (ERROR_SUCCESS == ::RegQueryValueEx(hKeyResult, wstrKey.c_str(), 0, &dwDataType, (LPBYTE)lpValue, &dwSize))
            {
                std::wstring wstrValue(lpValue);
                strValue = ws2s(wstrValue);
            }
            delete[] lpValue;
            break;
        }
        default:
            break;
        }
    }

    //关闭注册表
    ::RegCloseKey(hKeyResult);
    return strValue;
}

void SetRegValue(int nKeyType, const std::string& strUrl, const std::string& strKey, const std::string& value)
{
    HKEY hKey = NULL;
    HKEY rootKey = NULL;
    std::wstring wstrUrl = s2ws(strUrl);
    std::wstring wstrKey = s2ws(strKey);
    DWORD state=0, dwtype=0;
    char reBuff[buffSize] = { 0 };
    switch (nKeyType)
    {
    case 0:
    {
        rootKey = HKEY_CLASSES_ROOT;
        break;
    }
    case 1:
    {
        rootKey = HKEY_CURRENT_USER;
        break;
    }
    case 2:
    {
        rootKey = HKEY_LOCAL_MACHINE;
        break;
    }
    case 3:
    {
        rootKey = HKEY_USERS;
        break;
    }
    case 4:
    {
        rootKey = HKEY_PERFORMANCE_DATA;
        break;
    }
    case 5:
    {
        rootKey = HKEY_CURRENT_CONFIG;
        break;
    }
    case 6:
    {
        rootKey = HKEY_DYN_DATA;
        break;
    }
    case 7:
    {
        rootKey = HKEY_CURRENT_USER_LOCAL_SETTINGS;
        break;
    }
    case 8:
    {
        rootKey = HKEY_PERFORMANCE_TEXT;
        break;
    }
    case 9:
    {
        rootKey = HKEY_PERFORMANCE_NLSTEXT;
        break;
    }
    default:
    {
        rootKey = HKEY_LOCAL_MACHINE;
    }
    }
    auto lRet1 = RegCreateKeyEx(rootKey, wstrUrl.data(), 0, NULL, 0, 0, NULL, &hKey, &state);
    if (lRet1 == ERROR_SUCCESS)
    {
        if (state == REG_CREATED_NEW_KEY)
        {
            //LOG HERE
        }
        //关闭键
        RegCloseKey(hKey);
    }
    auto lRet = RegOpenKeyEx(rootKey, wstrUrl.data(), 0, KEY_WRITE, &hKey);
    if (lRet == ERROR_SUCCESS)
    {
        RegSetValueEx(hKey, wstrKey.c_str(), 0, REG_SZ, (BYTE*)s2ws(value).data(), buffSize);
        RegCloseKey(hKey);
    }
}