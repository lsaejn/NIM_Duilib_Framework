#include "pch.h"
#include "FileSystem.h"
#include <stdexcept>
#include <cwctype>
#include "../../base/util/string_util.h"

#include "Shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")


namespace Alime
{
	namespace FileSystem
	{
		FilePath::FilePath()
		{
			Initialize();
		}

		FilePath::FilePath(const String& _filePath)
			:fullPath_(_filePath)
		{
			Initialize();
		}

		FilePath::FilePath(const wchar_t* _filePath)
			:fullPath_(_filePath)
		{
			Initialize();
		}

		FilePath::FilePath(const FilePath& _filePath)
			:fullPath_(_filePath.GetFullPath())
		{
		}

		FilePath::~FilePath()
		{
		}

		void	FilePath::Initialize()
		{
			{
				std::vector<wchar_t> buffer(fullPath_.length() + 1);
				wcscpy_s(&buffer[0], fullPath_.length() + 1, fullPath_.c_str());
				for (size_t i = 0; i < buffer.size(); i++)
				{
					if (buffer[i] == L'\\' || buffer[i] == L'/')
					{
						buffer[i] = Delimiter;
					}
				}
				fullPath_ = &buffer[0];
			}
			if (fullPath_ != L"")
			{
				if (fullPath_.length() < 2 || fullPath_[1] != L':')
				{
					wchar_t buffer[MAX_PATH + 1] = { 0 };
					auto result = GetCurrentDirectoryW(sizeof(buffer) / sizeof(*buffer), buffer);
					if (result > MAX_PATH + 1 || result == 0)
					{
						throw std::exception("Failed to call GetCurrentDirectory.");
					}
					fullPath_ = String(buffer) + L"\\" + fullPath_;
				}
				{
					wchar_t buffer[MAX_PATH + 1] = { 0 };
					if (fullPath_.length() == 2 && fullPath_[1] == L':')
					{
						fullPath_ += L"\\";
					}
					auto result = GetFullPathNameW(fullPath_.data(), sizeof(buffer) / sizeof(*buffer), buffer, NULL);
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
					fullPath_ = buffer;
				}
			}
			if (fullPath_!= L"/" && fullPath_.length() > 0 && fullPath_[fullPath_.length() - 1] == Delimiter)
			{
				fullPath_ = fullPath_.substr(0, fullPath_.length() - 1);
			}
			fullPath_[0]=std::towupper(fullPath_[0]);
		}

		bool	FilePath::IsFile()const
		{
			//return PathFileExistsW(fullPath_.data())&&!PathIsDirectoryW(fullPath_.data());
			WIN32_FILE_ATTRIBUTE_DATA info;
			BOOL result = GetFileAttributesEx(fullPath_.c_str(), GetFileExInfoStandard, &info);
			if (!result) 
				return false;
			return (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
		}

		bool	FilePath::IsFolder()const
		{
			//return PathFileExistsW(fullPath_.data())&&PathIsDirectoryW(fullPath_.data());
			WIN32_FILE_ATTRIBUTE_DATA info;
			BOOL result = GetFileAttributesEx(fullPath_.c_str(), GetFileExInfoStandard, &info);
			if (!result) 
				return false;
			return (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;		
		}

		bool	FilePath::IsRoot()const
		{
			return fullPath_ == L"";
		}

		String  FilePath::GetName()const
		{
			auto index=fullPath_.find_last_of(Delimiter);
			if(index!=String::npos)
				return fullPath_.substr(index+1, fullPath_.length()-index);
			return fullPath_;
		}

		FilePath  FilePath::GetFolder()const
		{
			auto index=fullPath_.find_last_of(Delimiter);
			if(index!=String::npos)
				return fullPath_.substr(0,index);
			return FilePath();
		}

		String FilePath::GetFullPath()const
		{
			return fullPath_;
		}

		//不应该存在的api
		String FilePath::GetFullPathWithSurfix()const
		{
			if(IsFolder())
				return fullPath_+Delimiter;
			return fullPath_;
		}

		String FilePath::GetRelativePathFor(const FilePath& _filePath)
		{
			if (fullPath_.length() == 0 || _filePath.fullPath_.length() == 0 || fullPath_[0] != _filePath.fullPath_[0])
			{
				return _filePath.fullPath_;
			}
			wchar_t buffer[MAX_PATH + 1] = { 0 };
			PathRelativePathTo(
				buffer,
				fullPath_.c_str(),
				(IsFolder() ? FILE_ATTRIBUTE_DIRECTORY : 0),
				_filePath.fullPath_.c_str(),
				(_filePath.IsFolder() ? FILE_ATTRIBUTE_DIRECTORY : 0)
			);
			return buffer;
		}

		FilePath  FilePath::operator/(const String& relativePath)const
		{
			if (IsRoot())
			{
				return relativePath;
			}
			else
			{
				return fullPath_ + L"/" + relativePath;
			}
		}

		std::vector<String> FilePath::GetPathComponents(const String &path)
		{
			String pathRemaining = path;
			std::wstring delimiter;
			delimiter+=FilePath::Delimiter;
			auto re=nbase::StringTokenize(path.c_str(), delimiter.c_str());
			return { re.cbegin() ,re.cend() };
		}

		int  FilePath::Compare(const FilePath& a, const FilePath& b)
		{
			String strA = a.fullPath_;
			String strB = b.fullPath_;
			const wchar_t* bufA = strA.c_str();
			const wchar_t* bufB = strB.c_str();
			int length = strA.length() < strB.length() ? strA.length(): strB.length();
			while (length--)
			{
				int diff = *bufA++ - *bufB++;
				if (diff != 0)
				{
					return diff;
				}
			};
			return strA.length() - strB.length();
		}


		////File///////////////////////////////////////////////////////////////////////////////////////////////////////////
		File::File()
		{
		}

		File::File(const FilePath& _filePath)
			:filePath(_filePath)
		{
		}

		File::~File()
		{
		}

		const FilePath& File::GetFilePath()const
		{
			return filePath;
		}

		bool File::ReadAllTextWithEncodingTesting(String& /*text*/, Encoding& /*encoding*/, bool& /*containsBom*/)
		{
			return true;
		}

		String File::ReadAllTextByBom()const
		{
			String text;
			return text;
		}

		bool File::ReadAllTextByBom(String& /*text*/)const
		{
			return true;
		}

		bool File::ReadAllLinesByBom(std::vector<String>& /*lines*/)const
		{
			return true;
		}

		bool File::WriteAllText(const String& /*text*/, bool /*bom*/, Encoding /*encoding*/)
		{
			return true;
		}

		bool File::WriteAllLines(std::vector<String>& /*lines*/, bool /*bom*/, Encoding /*encoding*/)
		{
			return true;
		}

		bool File::Exists()const
		{
			return filePath.IsFile();
		}

		bool File::Delete()const
		{
			return DeleteFile(filePath.GetFullPath().data()) != 0;
		}

		bool File::Rename(const String& newName)const
		{
			String oldFileName = filePath.GetFullPath();
			String newFileName = (filePath.GetFolder() / newName).GetFullPath();
			return MoveFile(oldFileName.data(), newFileName.data()) != 0;
		}

/////////////////////folder/////////////////////////////////////////////////////

		Folder::Folder()
		{
		}

		Folder::Folder(const FilePath& _filePath)
			:filePath(_filePath)
		{
		}

		Folder::~Folder()
		{
		}

		const FilePath& Folder::GetFilePath()const
		{
			return filePath;
		}

		bool Folder::GetFolders(std::vector<Folder>& folders)const
		{
			if (filePath.IsRoot())
			{
				auto bufferSize = GetLogicalDriveStrings(0, nullptr);
				if (bufferSize > 0)
				{
					std::vector<wchar_t> buffer(bufferSize);
					if (GetLogicalDriveStrings((DWORD)buffer.size(), &buffer[0]) > 0)
					{
						auto begin = &buffer[0];
						auto end = begin + buffer.size();
						while (begin < end && *begin)
						{
							String driveString = begin;
							begin += driveString.length() + 1;
							folders.push_back(Folder(FilePath(driveString)));
						}
						return true;
					}
				}
				return false;
			}
			else
			{
				if (!Exists()) return false;
				WIN32_FIND_DATA findData;
				HANDLE findHandle = INVALID_HANDLE_VALUE;

				while (true)
				{
					if (findHandle == INVALID_HANDLE_VALUE)
					{
						String searchPath = (filePath / L"*").GetFullPath();
						findHandle = FindFirstFile(searchPath.data(), &findData);
						if (findHandle == INVALID_HANDLE_VALUE)
						{
							break;
						}
					}
					else
					{
						BOOL result = FindNextFile(findHandle, &findData);
						if (result == 0)
						{
							FindClose(findHandle);
							break;
						}
					}

					if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0)
						{
							folders.push_back(Folder(filePath / findData.cFileName));
						}
					}
				}
				return true;
			}
		}

		bool Folder::GetFiles(std::vector<File>& files)const
		{
			if (filePath.IsRoot())
			{
				return true;
			}
			if (!Exists()) return false;
			WIN32_FIND_DATA findData;
			HANDLE findHandle = INVALID_HANDLE_VALUE;

			while (true)
			{
				if (findHandle == INVALID_HANDLE_VALUE)
				{
					String searchPath = (filePath / L"*").GetFullPath();
					findHandle = FindFirstFile(searchPath.data(), &findData);
					if (findHandle == INVALID_HANDLE_VALUE)
					{
						break;
					}
				}
				else
				{
					BOOL result = FindNextFile(findHandle, &findData);
					if (result == 0)
					{
						FindClose(findHandle);
						break;
					}
				}

				if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					files.push_back(File(filePath / findData.cFileName));
				}
			}
			return true;
		}

		bool Folder::Exists()const
		{
			return filePath.IsFolder();
		}

		bool Folder::Create(bool recursively)const
		{
			if (recursively)
			{
				auto folder = filePath.GetFolder();
				if (folder.IsFile()) return false;
				if (folder.IsFolder()) return Create(false);
				return Folder(folder).Create(true) && Create(false);
			}
			else
			{
				return CreateDirectory(filePath.GetFullPath().data(), NULL) != 0;
			}
		}

		bool Folder::Delete(bool recursively)const
		{
			if (!Exists()) return false;

			if (recursively)
			{
				std::vector<Folder> folders;
				GetFolders(folders);
				for(auto &folder: folders)
				{
					if (!folder.Delete(true)) 
						return false;
				}

				std::vector<File> files;
				GetFiles(files);
				for(auto &file:files)
				{
					if (!file.Delete()) return false;
				}

				return Delete(false);
			}
			return RemoveDirectory(filePath.GetFullPath().data()) != 0;
		}

		bool Folder::Rename(const String& newName)const
		{
			String oldFileName = filePath.GetFullPath();
			String newFileName = (filePath.GetFolder() / newName).GetFullPath();
			return MoveFile(oldFileName.data(), newFileName.data()) != 0;
		}
	}
}