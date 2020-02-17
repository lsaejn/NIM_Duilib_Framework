#include "stdafx.h"
#include "MyResourceHandler.h"

MyResourceHandler::MyResourceHandler()
{

}


bool MyResourceHandler::ProcessRequest(CefRefPtr<CefRequest> request,
    CefRefPtr<CefCallback> callback)
{
    std::string url = request->GetURL();

    data_ = "hello cef";//返回到页面中的内容
    callback->Continue();//这个一定要有
    return true;//
}

void MyResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
    int64& response_length,
    CefString& redirectUrl) {

    response->SetMimeType("text/plain");
    response->SetStatus(200);
    response_length = data_.length();
}


bool MyResourceHandler::ReadResponse(void* data_out,
    int bytes_to_read,
    int& bytes_read,
    CefRefPtr<CefCallback> callback)
{
    int size = static_cast<int>(data_.length());
    memcpy(data_out, data_.c_str(), size);
    bytes_read = size;
    return true;
}

bool MyResourceHandler::CanGetCookie(const CefCookie& cookie)
{
    return false;
}

bool MyResourceHandler::CanSetCookie(const CefCookie& cookie)
{
    return false;
}
