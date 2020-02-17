#pragma once
#include "include\cef_scheme.h"
class MySchemeHandlerFactory :
	public CefSchemeHandlerFactory
{
public:
    virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& scheme_name,
        CefRefPtr<CefRequest> request)
        OVERRIDE;// Return a new resource handler instance to handle the request.


private:
    IMPLEMENT_REFCOUNTING(MySchemeHandlerFactory);
};

