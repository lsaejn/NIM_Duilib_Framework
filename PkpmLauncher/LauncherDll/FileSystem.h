#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include "windows.h"


using String = std::wstring;

namespace Alime
{
	namespace FileSystem
	{
		class FilePath
		{
		protected:
			String fullPath_;
			void	Initialize();
		public:
			static const wchar_t Delimiter = L'\\';
			FilePath();
			FilePath(const String& _filePath);
			FilePath(const wchar_t* _filePath);
			FilePath(const FilePath& _filePath);
			FilePath	operator/(const String& relativePath)const;
			~FilePath();

			FilePath GetFolder() const;
			static std::vector<String> GetPathComponents(const String& path);
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
			String GetName() const;
			String GetFullPath() const;	
			//fix me，不应该使用的函数
			String GetFullPathWithSurfix() const;
			String GetRelativePathFor(const FilePath& _filePath);
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
			String ReadAllTextByBom()const;
			bool	ReadAllTextWithEncodingTesting(String& text, Encoding& encoding, bool& containsBom);		
			bool	ReadAllTextByBom(String& text)const;
			bool	ReadAllLinesByBom(std::vector<String>& lines)const;
			bool	WriteAllText(const String& text, bool bom = true, Encoding encoding = Encoding::Utf16);
			bool	WriteAllLines(std::vector<String>& lines, bool bom = true, Encoding encoding = Encoding::Utf16);
			bool	Exists()const;
			bool	Delete()const;
			bool	Rename(const String& newName)const;
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
			bool	Rename(const String& newName)const;
		};

		static bool PathNameDetail(const FilePath &path, String& SensitivePath)
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
				String searchPath = path.GetFullPath();
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
		static bool PathNameCaseSensitive(const T& arg, String& SensitivePath)
		{
			if (!arg.Exists())
				return false;
			return PathNameDetail(arg.GetFilePath(), SensitivePath);
		}

		//fix me, 2020/03/30 最后一级目录很长的话，没办法处理
		//criticalLength不包括盘符
		static std::wstring GetAbbreviatedPath(const std::wstring& _path, size_t criticalLength= 60)
		{
			std::wstring result;
			auto path = _path;
			auto 	components=FileSystem::FilePath::GetPathComponents(path);
			if (components.size() == 1)
				return _path;
			auto lastFolder = components.back();
			if (lastFolder.length() > criticalLength)
			{		
				result = components.front() + FileSystem::FilePath::Delimiter;
				result += L"...";
				lastFolder = lastFolder.substr(lastFolder.length() - criticalLength);
				result += lastFolder;
				return result;
			}
			else
			{
				for (int i = components.size() - 1; i >= 1; --i)
				{
					if (components[i].size() + result.size() <= criticalLength)
					{
						result.insert(0, L"\\" + components[i]);
					}
					else
					{
						result.insert(0, L"\\...");
						break;
					}
				}
				result.insert(0, components[0]);
				return result;
			}			
		}

		//bool hasWriteAccessToFolder(std::wstring probe)
	} //namespace FileSystem
}//namespace Alime
