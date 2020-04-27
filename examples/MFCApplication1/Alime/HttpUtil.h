#pragma once
#include <vector>
#include <unordered_map>
#include <string>

#ifdef _MSC_VER
#define ALIME_USE_UNICODE
#endif // UNICODE

#ifdef ALIME_USE_UNICODE
#ifndef  USE_ALIME_STRING
typedef std::wstring String;
#else
typedef Alime::Wstring String;
#endif //  WIN32
#else
#ifndef  USE_ALIME_STRING
typedef std::string String;
#else
typedef Alime::String String;
#endif //  WIN32
#endif




namespace Alime
{
	namespace HttpUtility
	{
		/*
		windows下使用winhttp，
		linux下使用curl
		*/
		class HttpRequest
		{		
			typedef std::vector<char>	BodyBuffer;
			typedef std::vector<String>	StringList;
			typedef std::unordered_map<String, String>	HeaderMap;
		public:
			String	server;
			int		port;
			String	query;
			bool	secure;
			String	username;
			String	password;
			String	method;
			String	cookie;
			BodyBuffer	body;
			String	contentType;
			StringList	acceptTypes;
			HeaderMap	extraHeaders;
			HttpRequest();
		public:
			virtual bool	SetHost(const String& inputQuery);
			virtual void	SetBodyUtf8(const String& bodyString);
		};

		class HttpResponse
		{
			typedef std::vector<char>		BodyBuffer;
		public:
			int	statusCode;
			BodyBuffer	body;
			String	cookie;
			HttpResponse();
			virtual String	GetBodyUtf8();
		};

		bool	HttpQuery(const HttpRequest& request, HttpResponse& response);
		String	UrlEncodeQuery(const String& query);
	}
}
