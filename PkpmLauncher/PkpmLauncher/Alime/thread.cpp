#include "pch.h"
#include "windows.h"
#include "process.h"

#include <string>
#include <iosfwd>
#include <assert.h>

#include "thread.h"
#include "countDownLatch.h"


namespace Alime
{
	// Set a Thread Name in Native Code
	// http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
#if defined(OS_WIN) && defined(COMPILER_MSVC)
	const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.
		LPCSTR szName; // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags; // Reserved for future use, must be zero.
	} THREADNAME_INFO;
#pragma pack(pop)
#endif



	struct launcher
	{
		typedef Alime::Thread::ThreadFunc ThreadFunc;
		ThreadFunc func_;
		Thrd_ty* thr_;
		CountDownLatch* latch_;
		launcher(ThreadFunc func ,Thrd_ty* tid, CountDownLatch* latch):
			func_(std::move(func)),//move?
			thr_(tid),
			latch_(latch)
		{
		}

		void runInThread()
		{
			(*thr_).tid= GetCurrentThreadId();
			thr_ = NULL;
			latch_->countDown();
			latch_ = NULL;
			try
			{
				func_();
			}
			catch (const std::exception& ex)
			{
				//LOG_ERR<<ex
				(void)ex;
				//UCANNOTSEEIT(ex);
				abort();
			}
		}
	};

	//////thread////////////////////////////////////////////////////
	int Thread::numCreated_ = 0;

	Thread::Thread()
		:func_()
		,state_(ThreadState::NotStarted)
	{
		Thr_set_null(Thr_);
		numCreated_++;
	}

	Thread::Thread(ThreadFunc func, bool startImmediately, const std::string& name)
		:func_(std::move(func))
		, state_(ThreadState::NotStarted),
		threadName_(name)
	{
		Thr_set_null(Thr_);
		numCreated_++;
		if (startImmediately)
		{
			start();
			assert(state_ == ThreadState::Running);
		}
	}

	Thread::Thread(Thread&& _Other)
		: Thr_(_Other.Thr_)
		, state_(_Other.state_)
	{	// move from _Other
		Thr_set_null(_Other.Thr_);
		_Other.state_ = ThreadState::NotStarted;
	}

	//正常人不会调用这个函数...
	Thread& Thread::operator=(Thread&& _Other)
	{	// move from _Other
		return (MoveThread(_Other));
	}

	//for swap
	Thread& Thread::MoveThread(Thread& _Other)
	{	// move from _Other
		if (joinable())
			std::terminate();
		Thr_ = _Other.Thr_;
		Thr_set_null(_Other.Thr_);
		return (*this);
	}

	void Thread::start()
	{
		if (state_ == ThreadState::Running)
			throw std::exception("thread already start");
		CountDownLatch latch(1);
		launcher* task = new launcher(func_, &Thr_, &latch);
		Thr_.handle = (HANDLE)_beginthreadex(0, 0, ThreadProcFunc, task, 0, &Thr_.tid);
		//如果 _beginthreadex未返回错误，则可以保证其为有效句柄
		//https://docs.microsoft.com/zh-cn/cpp/c-runtime-library/reference/beginthread-beginthreadex?view=vs-2019
		auto ret = (Thr_.handle == 0 ? _Thrd_error : _Thrd_success);
		if (_Thrd_success == ret)
		{
			latch.wait();//线程还没启动，thread本身就被销毁，或者用户调用tid
			state_ = ThreadState::Running;
			assert(Thr_.tid > 0);
			if (!threadName_.empty())
			{
				SetThreadName(Thr_.tid, threadName_.c_str());
			}
		}
		else
		{// error happened
			delete task;
		}

	}

	unsigned int __stdcall Thread::ThreadProcFunc(void* obj)
	{
		launcher* data = static_cast<launcher*>(obj);
		assert(data);
		data->runInThread();
		delete data;
		_endthreadex(0);
		return 0;
	}

	void Thread::SetThreadName(id thread_id, const char* name)
	{
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = name;
		info.dwThreadID = thread_id;
		info.dwFlags = 0;

		__try {
			RaiseException(MS_VC_EXCEPTION,
				0,
				sizeof(info) / sizeof(ULONG_PTR),
				(ULONG_PTR*)&info);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			//
		}
	}

	Thread::~Thread()
	{
		//running Thr_.tid==0 判断不出是join还是detach了
		if (state_ == ThreadState::Running && joinable())
		{
			detach();
		}
		else if (joinable())//bug
			std::terminate();
	}

	void Thread::swap(Thread& _Other)
	{	// swap with _Other
		std::swap(Thr_, _Other.Thr_);
	}

	unsigned int Thread::get_id() const
	{
		return Thr_.tid;
	}

	bool Thread::joinable() const
	{	//纯粹是为了和posixApi一致
		return (!Thr_is_null(Thr_));
	}

	int Thread::Thrd_join(Thrd_ty thr)
	{	/* return exit code when thread terminates */
		unsigned long res;
		if (WaitForSingleObjectEx(thr.handle, INFINITE, FALSE) == WAIT_FAILED
			|| GetExitCodeThread(thr.handle, &res) == 0)
			return (_Thrd_error);
		return (CloseHandle(thr.handle) == 0 ? _Thrd_error : _Thrd_success);
	}

	void Thread::join()
	{	// join thread
		if (!joinable())
			throw std::exception("joinable");
		if (get_id() == ::GetCurrentThreadId())
			throw std::exception("dead lock will occur");
		if (Thrd_join(Thr_) != _Thrd_success)
			throw std::exception("no such process");
		Thr_set_null(Thr_);
		state_ = ThreadState::Stopped;
	}

	int Thread::Thrd_detach(Thrd_ty thr)
	{	/* tell OS to release thread's resources when it terminates */
		return (CloseHandle(thr.handle) == 0 ? _Thrd_error : _Thrd_success);
	}

	void Thread::detach()
	{	// detach thread
		if (state_==ThreadState::NotStarted)
			throw std::exception("thread should started first");
		if (!joinable())
			throw std::exception("invalid argument, tid!=0");
		if(_Thrd_success!=Thrd_detach(Thr_))
			throw std::exception("detach failed");
		Thr_set_null(Thr_);
	}

	int Thread::numCreated()
	{
		return numCreated_;
	}

	unsigned int Thread::hardware_concurrency()
	{	// return number of hardware thread contexts
		 SYSTEM_INFO info;
		 GetNativeSystemInfo(&info);
		 return (info.dwNumberOfProcessors);
	}

	Thread::native_handle_type Thread::native_handle()
	{	// return Win32 HANDLE as void*
		return (Thr_.handle);
	}

	namespace this_thread
	{
		Thread::id get_id()
		{
			return GetCurrentThreadId();
		}

		void yield()
		{	
			// give up balance of time slice
			// or sleep(0)
			SwitchToThread();
		}

		void sleep_for(int milliseconds)
		{	
			Sleep(static_cast<unsigned long>(milliseconds));
		}

		//void sleep_until(Alime::Timestamp t)
		//{	// sleep until _Abs_time
		//	sleep_for(static_cast<int>((t - Timestamp::Now()).toMilliseconds()));
		//}

	}	// namespace this_thread
}


