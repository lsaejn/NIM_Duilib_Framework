#pragma once
#include <stdint.h>
#include <string>

namespace Alime
{
	class Console
	{
	public:
		static void CreateConsole();
		static void Write(const wchar_t* str, int32_t  length);
		static void Write(const wchar_t* str);
		static void Write(const std::wstring& str);
		static void WriteLine(const wchar_t* buffer);
		static void WriteLine(const std::wstring& string);
		static std::wstring Read();
		static std::wstring ReadLine();
		static void SetColor(bool red, bool green, bool blue, bool light);
		static void SetTitle(const std::wstring& string);
		static void Clear();
		static void SetWindowPosition(int left, int top);
		static void SetWindowSize(int width, int height);
	};
}