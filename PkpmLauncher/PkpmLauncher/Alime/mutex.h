#pragma once
#include "windows.h"
#include "winbase.h"
#include <exception>
#include "assert.h"
#include "Alime/NonCopyable.h"

namespace Alime
{
	class MutexRW:public noncopyable
	{
	public:
		MutexRW()//:holder_(0)
		{
			InitializeSRWLock(&m_srw_lock);
		}

		~MutexRW()
		{
		}
		MutexRW(const MutexRW&) = delete;
		MutexRW& operator=(const MutexRW&) = delete;

		virtual void lock()
		{
			AcquireSRWLockExclusive(&m_srw_lock);
		}

		virtual bool try_lock()
		{
			//If the lock is successfully acquired, the return value is nonzero.
			bool ret= TryAcquireSRWLockExclusive(&m_srw_lock) != 0;
			return ret;
		}

		virtual bool try_lock_for(unsigned int seconds)
		{
			return try_lock();
		}

		virtual void unlock()
		{
			ReleaseSRWLockExclusive(&m_srw_lock);		
		}

		PSRWLOCK native_handle()
		{
			return &m_srw_lock;
		}

	private:
		SRWLOCK m_srw_lock;
	};

	class ConditionRW :public noncopyable
	{
	public:
		ConditionRW()
		{
			InitializeConditionVariable(&m_condition_variable);
		}

		~ConditionRW()
		{
		}

		ConditionRW(const ConditionRW&) = delete;
		ConditionRW& operator=(const ConditionRW&) = delete;

		virtual void destroy() {}

		virtual void wait(MutexRW *lock)
		{
			if (!wait_for(lock, INFINITE))
				std::terminate();
		}

		virtual bool wait_for(MutexRW *lock, unsigned int timeout)
		{
			return SleepConditionVariableSRW(&m_condition_variable, static_cast<MutexRW *>(lock)->native_handle(), timeout, 0) != 0;
		}

		virtual void notify_one()
		{
			WakeConditionVariable(&m_condition_variable);
		}

		virtual void notify_all()
		{
			WakeAllConditionVariable(&m_condition_variable);
		}

	private:
		CONDITION_VARIABLE m_condition_variable;
	};

	class Mutex :public noncopyable
	{
	public:
		Mutex()//:holder_(0)
		{
			::InitializeCriticalSectionAndSpinCount(&cs_, 2000);
		}

		~Mutex()
		{
			destroy();
		}
		Mutex(const Mutex&) = delete;
		Mutex& operator=(const Mutex&) = delete;

		virtual void destroy()
		{

		}

		virtual void lock()
		{
			::EnterCriticalSection(&cs_);
		}

		virtual bool try_lock()
		{
			if (::TryEnterCriticalSection(&cs_))
			{
				return true;
			}
			return false;
		}

		virtual bool try_lock_for(unsigned int seconds)
		{
			auto TimeEnterIn = ::GetTickCount64();
			while (::GetTickCount64() - TimeEnterIn < seconds * 1000)
			{
				if (try_lock())
				{
					return true;
				}
			}
			return false;
		}

		virtual void unlock()
		{
			::DeleteCriticalSection(&cs_);
		}

		void* native_handle()
		{
			return &cs_;
		}

	private:
		CRITICAL_SECTION cs_;
	};

	class Condition :public noncopyable
	{
	public:
		Condition()
		{
			InitializeConditionVariable(&cv_);
		}

		~Condition()
		{
		}

		Condition(const Condition&) = delete;
		Condition& operator=(const Condition&) = delete;

		virtual void wait(Mutex* lock)
		{
			if (!wait_for(lock, INFINITE))
				std::terminate();
		}
		
		virtual bool wait_for(Mutex* lock, unsigned int timeout)
		{
			if (timeout < 0)
				throw "invalid_argument: timeout";
			return SleepConditionVariableCS(&cv_, 
				static_cast<CRITICAL_SECTION*>(lock->native_handle()), timeout*1000);
		}

		virtual void notify_one()
		{
			WakeConditionVariable(&cv_);
		}

		virtual void notify_all()
		{
			WakeAllConditionVariable(&cv_);
		}

	private:
		CONDITION_VARIABLE cv_;
	};

	template<typename T>
	class Lock_guard :public noncopyable
	{
	public:
		Lock_guard( T& lock)
			:lock_(lock)
		{
			lock_.lock();
		}
		~Lock_guard()
		{
			lock_.unlock();
		}

		T& Get()
		{
			return lock_;
		}

	private:
		 T& lock_;
	};
}
