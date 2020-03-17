#include "pch.h"
#include "FileSystem.h"
#include <stdexcept>
#include <cwctype>
#include "windows.h"
#include "Shlwapi.h"

namespace Alime
{
	namespace FileSystem
	{
		FilePath::FilePath()
		{
			Initialize();
		}

		FilePath::FilePath(const WString& _filePath)
			:fullPath(_filePath)
		{
			Initialize();
		}

		FilePath::FilePath(const wchar_t* _filePath)
			:fullPath(_filePath)
		{
			Initialize();
		}

		FilePath::FilePath(const FilePath& _filePath)
			:fullPath(_filePath.GetFullPath())
		{
		}

		FilePath::~FilePath()
		{
		}

		void	FilePath::Initialize()
		{
			{
				std::vector<wchar_t> buffer(fullPath.length() + 1);
				wcscpy_s(&buffer[0], fullPath.length() + 1, fullPath.c_str());
				for (size_t i = 0; i < buffer.size(); i++)
				{
					if (buffer[i] == L'\\' || buffer[i] == L'/')
					{
						buffer[i] = Delimiter;
					}
				}
				fullPath = &buffer[0];
			}
			if (fullPath != L"")
			{
				if (fullPath.length() < 2 || fullPath[1] != L':')
				{
					wchar_t buffer[MAX_PATH + 1] = { 0 };
					auto result = GetCurrentDirectoryW(sizeof(buffer) / sizeof(*buffer), buffer);
					if (result > MAX_PATH + 1 || result == 0)
					{
						throw std::exception("Failed to call GetCurrentDirectory.");
					}
					fullPath = WString(buffer) + L"\\" + fullPath;
				}
				{
					wchar_t buffer[MAX_PATH + 1] = { 0 };
					if (fullPath.length() == 2 && fullPath[1] == L':')
					{
						fullPath += L"\\";
					}
					auto result = GetFullPathNameW(fullPath.data(), sizeof(buffer) / sizeof(*buffer), buffer, NULL);
					if (result > MAX_PATH + 1 || result == 0)
					{
						throw std::exception("The path is illegal.");
					}
					{
						wchar_t shortPath[MAX_PATH + 1];
						wchar_t longPath[MAX_PATH + 1];
						if (GetShortPathNameW(buffer, shortPath, MAX_PATH) > 0)
						{
							if (GetLongPathNameW(shortPath, longPath, MAX_PATH) > 0)
							{
								memcpy(buffer, longPath, sizeof(buffer));
							}
						}
					}
					fullPath = buffer;
				}
			}
			if (fullPath!= L"/" && fullPath.length() > 0 && fullPath[fullPath.length() - 1] == Delimiter)
			{
				fullPath = fullPath.substr(0, fullPath.length() - 1);
			}
			fullPath[0]=std::towupper(fullPath[0]);
		}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//以下"部分"未完成
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////



		bool	FilePath::sFile()const
		{
			return PathFileExistsW(fullPath.data())&&!PathIsDirectoryW(fullPath.data());
		}

		bool	FilePath::IsFolder()const
		{
			return PathFileExistsW(fullPath.data())&&PathIsDirectoryW(fullPath.data());
		}

		bool	FilePath::IsRoot()const
		{
			return fullPath == L"";
		}

		WString  FilePath::GetName()const
		{
			auto index=fullPath.find_last_of(Delimiter);
			if(index!=String::npos)
				return fullPath.substr(index, fullPath.length()-index);
			return fullPath;
		}

		FilePath  FilePath::GetFolder()const
		{
			auto index=fullPath.find_last_of(Delimiter);
			if(index!=String::npos)
				return fullPath.substr(0,index);
			return FilePath();
		}

		WString FilePath::GetFullPath()const
		{
			return fullPath;
		}

		WString FilePath::GetFullPathWithSurfix()const
		{
			return fullPath+L'\\';
		}

		// Fix me
		WString FilePath::GetRelativePathFor(const FilePath& _filePath)
		{
			return fullPath;
		}

		// Fix me
		FilePath  FilePath::operator/(const WString& relativePath)const
		{
			return FilePath();
		}

		// Fix me
		void	FilePath::GetPathComponents(WString path, const std::vector<WString>& components)
		{
			
		}

		// Fix me
		int  FilePath::Compare(const FilePath& a, const FilePath& b)
		{
			return 0;
		}

/*					
			bool	operator==(const FilePath& filePath)const{ return Compare(*this, filePath) == 0; }
			bool	operator!=(const FilePath& filePath)const{ return Compare(*this, filePath) != 0; }
			bool	operator< (const FilePath& filePath)const{ return Compare(*this, filePath) <  0; }
			bool	operator<=(const FilePath& filePath)const{ return Compare(*this, filePath) <= 0; }
			bool	operator> (const FilePath& filePath)const{ return Compare(*this, filePath) >  0; }
			bool	operator>=(const FilePath& filePath)const{ return Compare(*this, filePath) >= 0; }	
*/

	}
}