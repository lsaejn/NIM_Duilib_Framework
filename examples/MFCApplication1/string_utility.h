#pragma once
#ifndef  _STRING_UTILITY_H_PKPM
#define _STRING_UTILITY_H_PKPM

#include <algorithm>
#include <string>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>      /* Needed only for _O_RDWR definition */
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <share.h>
#include <string>
#include <iosfwd>    // for ostream forward-declaration
#include <vector>
#include <string>


template<typename T>
void IPRETENDTONOTUSEIT(T&&)
{
	;;;  ;;;;   ;;;; ;;;  ;;;;;;;
	;;                     ;;
	;  ;;;;;;;      ;;;;;;;    ;;;;;
	;       ;            ;;;;; 
	;     ;;;;;         ;    
	;              ;   
	;;;;;;;;;;;;;;;;;
}

/*you see this because some company
is not intented to update to c++11. that
means 'it' must tolerant these ugly code
just like i did.
*/
#define FalseIsSometimesCouldBeTolerable(arg) \
	if(!arg) \
	return;

template<typename F>
void FalseIsFuckingNotTolerable(F&& f)
{
	if (!f())
		throw("bad return value");
}

template<typename F>
void FunctionIgnoreError(F&& f)
{
	try
	{
		f();
	}
	catch (...)
	{
		//LOG_ERROR<<""
	}
}
// in c++11 you will can get a better choice
//template<typename F,typename... Args>
//void returnFalseIsFuckingNotTolerable(F&& f, Args&&... args)
//{
//	assert(f(args...));
//}

//二等公民
std::string KS_UTF8_to_ANSI_v31 (const char* szUTF8);
namespace string_utility
{
	class StringPiece;
	//commonutil 写到vector里是闹哪样
	std::wstring	AnsiToUnicode(const std::string &);
	std::string		UnicodeToUTF8(const std::wstring& );
	std::string		AnsiToUtf8(const std::string &);

	std::string		WStringToString(const std::wstring &wstr);
	std::wstring	Utf8ToUnicode(const std::string& strUtf8);
	WCHAR*		Utf8ToUnicode(const CHAR * lpszStr);

	std::string		strerror_Alime(int e);

	std::string replace_all(std::string&& s, const std::string& search, const std::string& rep);
	std::vector<std::string> string_split(const std::string& s,  std::string&& c);
	std::vector<std::string> string_split(const StringPiece& s, const StringPiece& c);
	bool startWith(const char* src, const char* des);
	bool endWith(const char* src, const char* des);

	template<typename Ty, int nSize>
	int ArraySize(Ty(&Arg)[nSize])
	{
		return nSize;
	}


	class ReadSmallFile
	{
	public:
		ReadSmallFile(const std::string& filename)
			: fd_(::_open(filename.c_str(), O_RDONLY)),
			err_(0)
		{
			buf_[0] = '\0';
			if (fd_ < 0)
			{
				err_ = errno;
			}
		}

		~ReadSmallFile()
		{
			if (fd_ >= 0)
			{
				::close(fd_); // FIXME: check EINTR
			}
		}






		int readToString(std::string& content)
		{
			char sizeok[sizeof(off_t) == 4 ? 1 : -1];
			IPRETENDTONOTUSEIT(sizeok);
			int err = err_;
			if (fd_ >= 0)
			{
				content.clear();
				struct stat statbuf;
				int fileSize = 0;
				if (::fstat(fd_, &statbuf) == 0)
				{
					if ((_S_IFMT&statbuf.st_mode) == S_IFREG)
					{
						fileSize = statbuf.st_size;//FIX ME
						content.reserve(fileSize);
					}
					else
						err = EISDIR;
				}
				else
					err = errno;
				while (1)
				{
					int n = ::read(fd_, buf_, kBufferSize);
					if (n > 0)
						content.append(buf_, n);
					else
					{
						if (n < 0)
							err = errno;
						break;
					}
				}
			}
			return err;
		}

		const char* buffer() const
		{
			return buf_;
		}
		static const int kBufferSize = 64 * 1024;
	private:
		int fd_;
		int err_;
		char buf_[kBufferSize];
	};

	/*
	    google class 
	*/
	class StringPiece
	{
	public:
		StringPiece()
			: ptr_(NULL), length_(0)
		{
		}
		StringPiece(const char* str)
			: ptr_(str), 
			length_(static_cast<int>(strlen(ptr_)))
		{
		}
		StringPiece(const unsigned char* str)
			: ptr_(reinterpret_cast<const char*>(str)),
			length_(static_cast<int>(strlen(ptr_)))
		{
		}
		StringPiece(const std::string& str)
			: ptr_(str.data()), length_(static_cast<int>(str.size()))
		{
		}
		StringPiece(const char* offset, int len)
			: ptr_(offset), length_(len)
		{
		}
		const char* data() const
		{
			return ptr_;
		}
		int size() const
		{
			return length_;
		}
		bool empty() const
		{
			return length_ == 0;
		}
		const char* begin() const
		{
			return ptr_;
		}
		const char* end() const
		{
			return ptr_ + length_;
		}
		void clear()
		{
			ptr_ = NULL;
			length_ = 0;
		}
		void set(const char* buffer, int len)
		{
			ptr_ = buffer;
			length_ = len;
		}
		void set(const char* str)
		{
			ptr_ = str;
			length_ = static_cast<int>(strlen(str));
		}
		void set(const void* buffer, int len)
		{
			ptr_ = reinterpret_cast<const char*>(buffer);
			length_ = len;
		}
		char operator[](int i) const
		{
			return ptr_[i];
		}
		void remove_prefix(int n)
		{
			ptr_ += n;
			length_ -= n;
		}
		void remove_suffix(int n)
		{
			length_ -= n;
		}
		bool operator==(const StringPiece& x) const
		{
			return ((length_ == x.length_) &&
				(memcmp(ptr_, x.ptr_, length_) == 0));
		}
		bool operator!=(const StringPiece& x) const
		{
			return !(*this == x);
		}

		int compare(const StringPiece& x) const
		{
			int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_);
			if (r == 0) 
			{
				if (length_ < x.length_) r = -1;
				else if (length_ > x.length_) r = +1;
			}
			return r;
		}

		std::string as_string() const
		{
			return std::string(data(), size());
		}

		void CopyToString(std::string* target) const
		{
			target->assign(ptr_, length_);
		}


		bool starts_with(const StringPiece& x) const
		{
			return ((length_ >= x.length_) && (memcmp(ptr_, x.ptr_, x.length_) == 0));
		}

	private:
		const char*   ptr_;
		int           length_;
	};

	std::ostream& operator<<(std::ostream& o, const string_utility::StringPiece& piece);
};

#endif

