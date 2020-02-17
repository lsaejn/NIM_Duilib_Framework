#include "StdAfx.h"
#include "string_utility.h"



namespace string_utility
{
	std::wstring AnsiToUnicode(const std::string &strAnsi)
	{
		int size= ::MultiByteToWideChar(CP_ACP,0,strAnsi.c_str(),-1,NULL,0);
		wchar_t* pUnicode = new wchar_t[size + 1];
		memset((void*)pUnicode, 0, (size + 1) * sizeof(wchar_t));
		::MultiByteToWideChar(CP_ACP,0,strAnsi.c_str(),-1,pUnicode,size);
		std::wstring  strUnicode(pUnicode);
		delete[] pUnicode;
		return strUnicode;
	}

	std::string UnicodeToUTF8(const std::wstring& w)
	{
		std::string result;
		auto size = WideCharToMultiByte(CP_UTF8,0,w.c_str(),-1,NULL,0,NULL,NULL);
		if(!size) return result;
		result.reserve(size-1);
		WideCharToMultiByte(CP_UTF8,0,w.c_str(),-1,&result.front(),size-1,NULL,NULL);
		return result;
	}

	std::string AnsiToUtf8(const std::string &ansi)
	{
		std::wstring w = AnsiToUnicode(ansi);
		return UnicodeToUTF8(w);
	}

	std::vector<std::string> string_split(const std::string& s, std::string&& c)
	{
		std::vector<std::string> result;
		size_t len = s.length();
		std::string::size_type pos1 = 0;
		std::string::size_type pos2 = s.find(c);	
		while (std::string::npos != pos2)
		{
			result.push_back(s.substr(pos1, pos2 - pos1));
			pos1 = pos2 + c.size();
			pos2 = s.find(c, pos1);
		}
		if (pos1 != len)
			result.push_back(s.substr(pos1));
		return result;
	}

	std::vector<std::string> string_split(const StringPiece& s, const StringPiece& c)
	{
		std::vector<std::string> result;
		if(s.empty()) 
			return result;
		std::string copy(s.as_string());
		char* token=strtok(&*copy.begin(),c.data());
		while(token)
		{
			result.push_back(token);
			token=strtok(NULL,c.data());
		}
		return result;
	}

	bool startWith(const char* src, const char* des)
	{
		return (strlen(des) <= strlen(src)) && 
			(!memcmp(src,des,strlen(des)) );
	}

	bool endWith(const char* src, const char* des)
	{
		int offset=strlen(src) - strlen(des);
		return  offset>=0
			&& startWith(src+offset, des);
	}

	std::string replace_all(std::string&& s, const std::string& search, const std::string& rep)
	{
		size_t pos = 0;
		while ((pos = s.find(search, pos)) != std::string::npos)
		{
			s.replace(pos, search.size(), rep);
			pos += rep.size();
		}
		return std::move(s);
	}

	std::wstring Utf8ToUnicode(const std::string& strUtf8)
	{
		std::wstring strUnicode;

		WCHAR * lpszUnicode = Utf8ToUnicode(strUtf8.c_str());
		if (lpszUnicode != NULL)
		{
			strUnicode = lpszUnicode;
			delete[]lpszUnicode;
		}

		return strUnicode;
	}

	WCHAR * Utf8ToUnicode(const CHAR * lpszStr)
	{
		WCHAR * lpUnicode;
		int nLen;

		if (NULL == lpszStr)
			return NULL;

		nLen = ::MultiByteToWideChar(CP_UTF8, 0, lpszStr, -1, NULL, 0);
		if (0 == nLen)
			return NULL;

		lpUnicode = new WCHAR[nLen + 1];
		if (NULL == lpUnicode)
			return NULL;

		memset(lpUnicode, 0, sizeof(WCHAR) * (nLen + 1));
		nLen = ::MultiByteToWideChar(CP_UTF8, 0, lpszStr, -1, lpUnicode, nLen);
		if (0 == nLen)
		{
			delete[]lpUnicode;
			return NULL;
		}

		return lpUnicode;
	}


	namespace
	{
		const std::string empty_string;
	}
	

	std::string strerror_Alime(int e) 
	{
		LPVOID buf = nullptr;
		::FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, e, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buf, 0, nullptr);

		if (buf)
		{
			std::string s = (char*)buf;
			LocalFree(buf);
			return s;
		}
		return empty_string;
	}

	std::string	WStringToString(const std::wstring &wstr)
	{
		std::string result;
		int len=WideCharToMultiByte(CP_ACP,0,wstr.c_str(),wstr.size(),NULL,0,NULL,NULL);
		char* buff=new char[len+1];
		WideCharToMultiByte(CP_ACP,0,wstr.c_str(),wstr.size(),buff,len,NULL,NULL);
		buff[len]='\0';
		result.assign(buff);
		delete []buff;
		return result;
	}
}

std::string KS_UTF8_to_ANSI_v31 (const char* szUTF8)
{
	if (szUTF8 == NULL)
		return "" ;

	int     nLen = ::MultiByteToWideChar (CP_UTF8, 0, szUTF8, -1, NULL, 0) ;
	WCHAR   * pWstr = new WCHAR[nLen+1] ;
	ZeroMemory (pWstr, sizeof(WCHAR) * (nLen+1)) ;
	::MultiByteToWideChar (CP_UTF8, 0, szUTF8, -1, pWstr, nLen) ;
	std::string     strAnsi (_bstr_t((wchar_t*)pWstr)) ;
	delete[] pWstr ;
	return strAnsi ;
}
