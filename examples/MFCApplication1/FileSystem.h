#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include "windows.h"

using WString = std::wstring;

namespace Alime
{
	namespace FileSystem
	{
		class FilePath
		{
		protected:
			WString fullPath_;
			void	Initialize();
		public:
			static const wchar_t Delimiter = L'\\';
			FilePath();
			FilePath(const WString& _filePath);
			FilePath(const wchar_t* _filePath);
			FilePath(const FilePath& _filePath);
			FilePath	operator/(const WString& relativePath)const;
			~FilePath();

			FilePath GetFolder() const;
			static void GetPathComponents(WString path, std::list<WString>& components);
			static int	Compare(const FilePath& a, const FilePath& b);
			bool	operator==(const FilePath& filePath)const{ return Compare(*this, filePath) == 0; }
			bool	operator!=(const FilePath& filePath)const{ return Compare(*this, filePath) != 0; }
			bool	operator< (const FilePath& filePath)const{ return Compare(*this, filePath) <  0; }
			bool	operator<=(const FilePath& filePath)const{ return Compare(*this, filePath) <= 0; }
			bool	operator> (const FilePath& filePath)const{ return Compare(*this, filePath) >  0; }
			bool	operator>=(const FilePath& filePath)const{ return Compare(*this, filePath) >= 0; }
			bool	IsFile() const;
			bool	IsFolder() const;
			bool	IsRoot() const;
			WString GetName() const;
			WString GetFullPath() const;	
			//fix me，不应该使用的函数
			WString GetFullPathWithSurfix() const;
			WString GetRelativePathFor(const FilePath& _filePath);
		};

		class File
		{
		private:
			FilePath	 filePath;
		public:
			File();
			File(const FilePath& _filePath);
			~File();
			const FilePath& GetFilePath()const;
			enum Encoding
			{
				Mbcs,
				Utf8,
				Utf16,
				Utf16BE
			};
			WString ReadAllTextByBom()const;
			bool	ReadAllTextWithEncodingTesting(WString& text, Encoding& encoding, bool& containsBom);		
			bool	ReadAllTextByBom(WString& text)const;
			bool	ReadAllLinesByBom(std::vector<WString>& lines)const;
			bool	WriteAllText(const WString& text, bool bom = true, Encoding encoding = Encoding::Utf16);
			bool	WriteAllLines(std::vector<WString>& lines, bool bom = true, Encoding encoding = Encoding::Utf16);
			bool	Exists()const;
			bool	Delete()const;
			bool	Rename(const WString& newName)const;
		};

		class Folder
		{
		private:
			FilePath	 filePath;
		public:
			Folder();
			Folder(const FilePath& _filePath);
			~Folder();
			const FilePath& GetFilePath()const;
			bool	GetFolders(std::vector<Folder>& folders)const;
			bool	GetFiles(std::vector<File>& files)const;
			bool	Exists()const;
			bool	Create(bool recursively)const;
			bool	Delete(bool recursively)const;
			bool	Rename(const WString& newName)const;
		};

		static bool PathNameDetail(const FilePath &path, WString& SensitivePath)
		{
			Alime::FileSystem::Folder dir(path.GetFolder());
			if (dir.GetFilePath().IsRoot())
			{
				SensitivePath = path.GetFullPath();
				return true;
			}	
			WIN32_FIND_DATA findData;
			HANDLE findHandle = INVALID_HANDLE_VALUE;
			if (findHandle == INVALID_HANDLE_VALUE)
			{
				WString searchPath = path.GetFullPath();
				findHandle = FindFirstFile(searchPath.data(), &findData);
				if (findHandle == INVALID_HANDLE_VALUE)
					return false;
				else
				{
					SensitivePath=(dir.GetFilePath() / findData.cFileName).GetFullPathWithSurfix();
					return true;
				}
			}
			else //for while loop
				return false;
		}

		template<typename T>
		static bool PathNameCaseSensitive(const T& arg, WString& SensitivePath)
		{
			if (!arg.Exists())
				return false;
			return PathNameDetail(arg.GetFilePath(), SensitivePath);
		}

		//fix me, 2020/03/30 最后一级目录很长的话，没办法处理
		static void GetAbbreviatedPath(std::wstring& path, size_t criticalLength= 70, size_t prefixLength=0)
		{
			if (path.size() < criticalLength)
				return;
			auto firstDelimiter = path.find(FilePath::Delimiter);
			auto lastDelimiter = path.rfind(FilePath::Delimiter);
			assert(lastDelimiter == path.length() - 1);
			auto secondToLastDelimiter = path.rfind(FilePath::Delimiter, lastDelimiter-1);
			
			std::wstring result(path.substr(0, firstDelimiter + 1) + L"...");
			if (firstDelimiter != secondToLastDelimiter)
				result += L'\\';
			/*  like C:\\aa...aaa\\   */
			size_t lengthOfLastFolder = lastDelimiter - secondToLastDelimiter-1;
			if (lengthOfLastFolder < criticalLength - prefixLength)
			{
				result += path.substr(secondToLastDelimiter, lengthOfLastFolder + 2);
			}
			else
			{
				std::wstring toAppend;
				toAppend += L"...";
				for (int i = criticalLength - prefixLength; i >0; --i)
				{
					toAppend += path[path.size()- i];
				}
				result += toAppend;				
			}
			path = result;
		}

		//bool hasWriteAccessToFolder(std::wstring probe)


	} //namespace FileSystem
}//namespace Alime
