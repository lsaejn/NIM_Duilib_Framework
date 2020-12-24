#pragma once
#include <string>
#include <memory>
#include <windows.h>
#include <gdiplus.h>

#include "templates.h"
#include "assert.h"

using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")

/*
修正工程图片比例，该bug导致网页布局异常
*/
class ProjectPicZoomer
{
public:
	ProjectPicZoomer(const std::wstring& filePath)
		:filePath_(filePath),
		srcBitmap_(NULL)
	{
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&gdiToken_, &gdiplusStartupInput, NULL);
		srcBitmap_ = new Bitmap(filePath_.c_str());
	}
	~ProjectPicZoomer()
	{
		if(srcBitmap_)
			delete srcBitmap_;
		GdiplusShutdown(gdiToken_);
	}

	bool IsValidScale()
	{
		auto width = srcBitmap_->GetWidth();
		auto height = srcBitmap_->GetHeight();
		if (width * 1.0 / height >= 0.75)
			return true;
		return false;
	}

	bool ZoomAndSaveAs(const std::wstring& destFilePath, UINT destWidth, UINT destHeight, const std::wstring& destType)
	{
		auto ptrToDelete = ZoomThis(destWidth, destHeight);
		ScopePtrDeletor graphic(ptrToDelete.first);
		ScopePtrDeletor bitmap(ptrToDelete.second);
		UINT	numOfEncoders, sizeOfEncoders = 0;
		ImageCodecInfo* pImageCodecInfo = NULL;
		if (GetImageEncodersSize(&numOfEncoders, &sizeOfEncoders) != Ok
			|| sizeOfEncoders == 0)
			return false;
		pImageCodecInfo = (ImageCodecInfo*)(malloc(sizeOfEncoders));
		ScopePtrDeletor info(pImageCodecInfo);
		if (GetImageEncoders(numOfEncoders, sizeOfEncoders, pImageCodecInfo) != Ok)
			return false;
		for (UINT i = 0; i < numOfEncoders; ++i)
		{
			if (std::wstring(pImageCodecInfo[i].MimeType) == destType)
			{
				if (destFilePath== filePath_)
				{
					delete srcBitmap_;
					srcBitmap_ = nullptr;
				}
				ptrToDelete.second->Save(destFilePath.c_str(), &(pImageCodecInfo[i].Clsid));
				return true;
			}
		}
		return false;
	}
private:

	std::pair<Graphics*, Bitmap*> ZoomThis(UINT destWidth, UINT destHeight)
	{
		Bitmap* destBmpPtr = new Bitmap(destWidth, destHeight, PixelFormat32bppARGB);
		Graphics* graphics = Graphics::FromImage(destBmpPtr);
		graphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
		graphics->DrawImage(srcBitmap_, 0, 0, destWidth, destHeight);
		return { graphics, destBmpPtr };
	}

	std::wstring filePath_;
	Bitmap* srcBitmap_;
	ULONG_PTR gdiToken_;
};
