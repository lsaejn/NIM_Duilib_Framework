#pragma once
#include "include\cef_resource_handler.h"
#include <fstream>
#include <stdio.h>
class MyResourceHandler :
	public CefResourceHandler
{
public:
    MyResourceHandler();

    virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
        CefRefPtr<CefCallback> callback)OVERRIDE;

    virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
        int64& response_length,
        CefString& redirectUrl) OVERRIDE;

    virtual void Cancel() OVERRIDE {
        // Cancel the response...
    }

    virtual bool ReadResponse(void* data_out,
        int bytes_to_read,
        int& bytes_read,
        CefRefPtr<CefCallback> callback)
        OVERRIDE;

    virtual bool CanGetCookie(const CefCookie& cookie);


    virtual bool CanSetCookie(const CefCookie& cookie);

private:
    std::string data_;
    IMPLEMENT_REFCOUNTING(MyResourceHandler);
};

