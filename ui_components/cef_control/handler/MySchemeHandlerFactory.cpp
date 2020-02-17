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
    cef_uri_unescape_rule_t unescape_rule = UU_SPACES;//处理路径中有空格
    CefString& decoded_value = CefURIDecode(url, true, unescape_rule);

    CefURLParts urlParts;
    if (CefParseURL(url, urlParts))
    {
        std::wstring filePath = CefURIDecode(CefString(&urlParts.query), true, unescape_rule);//处理有中文路径
        CefRefPtr<CefStreamReader> fileStream = CefStreamReader::CreateForFile(CefString(filePath));

        if (fileStream != nullptr)
        {
            // "ext" 获取扩展名 例如："txt"、"jpg"
            std::wstring ext;
            ext = PathFindExtension(filePath.c_str());
            ext = ext.substr(1, ext.length());
            CefString mimeType(CefGetMimeType(ext));
            //todo: Complete known mime times with web-font extensions
            if (mimeType.empty())
            {
                //mimeType = "font/" + fileExtension;
            }

            return CefRefPtr<CefStreamResourceHandler>(new CefStreamResourceHandler(mimeType, fileStream));//以二进制流的方式把文件返给js并显示到html标签中
        }
    }

    return new MyResourceHandler();
}
