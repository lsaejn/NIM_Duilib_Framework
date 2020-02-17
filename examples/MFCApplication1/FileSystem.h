#pragma once
#include <vector>
#include <unordered_map>
#include <string>

//See HttpUitl.h

typedef std::wstring String;
typedef String WString;



namespace Alime
{
	namespace FileSystem
	{
		class FilePath
		{
		protected:
			WString	fullPath;
			void	Initialize();
			static void		GetPathComponents(WString path, const std::vector<WString>& components);
			static WString	ComponentsToPath(const std::vector<WString>& components);
		public:
			static const wchar_t		Delimiter = L'\\';
			FilePath();
			FilePath(const WString& _filePath);
			FilePath(const wchar_t* _filePath);
			FilePath(const FilePath& _filePath);
			FilePath	operator/(const WString& relativePath)const;
			~FilePath();

			FilePath	GetFolder()const;
			static int	Compare(const FilePath& a, const FilePath& b);
			bool	operator==(const FilePath& filePath)const{ return Compare(*this, filePath) == 0; }
			bool	operator!=(const FilePath& filePath)const{ return Compare(*this, filePath) != 0; }
			bool	operator< (const FilePath& filePath)const{ return Compare(*this, filePath) <  0; }
			bool	operator<=(const FilePath& filePath)const{ return Compare(*this, filePath) <= 0; }
			bool	operator> (const FilePath& filePath)const{ return Compare(*this, filePath) >  0; }
			bool	operator>=(const FilePath& filePath)const{ return Compare(*this, filePath) >= 0; }
			bool	sFile()const;
			bool	IsFolder()const;
			bool	IsRoot()const;
			WString	GetName()const;
			WString	GetFullPath()const;
			WString	GetFullPathWithSurfix()const;
			WString	GetRelativePathFor(const FilePath& _filePath);
		};

		class File
		{
			FilePath	 path_;
		};

		class Folder
		{
			FilePath	 path_;
		};
	}
}
