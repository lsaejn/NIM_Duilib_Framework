#include "stdafx.h"
#include "MySchemeHandlerFactory.h"

#include "MySchemeHandlerFactory.h"
#include "MyResourceHandler.h"
#include "include/cef_parser.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_stream_resource_handler.h"
#include <shlwapi.h> 
#pragma comment(lib, "shlwapi.lib")

CefRefPtr<CefResourceHandler> MySchemeHandlerFactory::Create(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request)
{
    CEF_REQUIRE_IO_THREAD();

    CefString url = request->GetURL();
    cef_uri_unescape_rule_t unescape_rule = UU_SPACES;//����·�����пո�
    CefString& decoded_value = CefURIDecode(url, true, unescape_rule);

    CefURLParts urlParts;
    if (CefParseURL(url, urlParts))
    {
        std::wstring filePath = CefURIDecode(CefString(&urlParts.query), true, unescape_rule);//����������·��
        CefRefPtr<CefStreamReader> fileStream = CefStreamReader::CreateForFile(CefString(filePath));

        if (fileStream != nullptr)
        {
            // "ext" ��ȡ��չ�� ���磺"txt"��"jpg"
            std::wstring ext;
            ext = PathFindExtension(filePath.c_str());
            ext = ext.substr(1, ext.length());
            CefString mimeType(CefGetMimeType(ext));
            //todo: Complete known mime times with web-font extensions
            if (mimeType.empty())
            {
                //mimeType = "font/" + fileExtension;
            }

            return CefRefPtr<CefStreamResourceHandler>(new CefStreamResourceHandler(mimeType, fileStream));//�Զ��������ķ�ʽ���ļ�����js����ʾ��html��ǩ��
        }
    }

    return new MyResourceHandler();
}
