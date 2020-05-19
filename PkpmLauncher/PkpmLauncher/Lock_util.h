#pragma once
#include <mutex>

template<class T>
struct LockGuardPtr;

template<class T>
struct LockGuarded
{
    friend struct LockGuardPtr<T>;
    LockGuardPtr<T> lock()
    {
        return *this;
    }

private:
    std::mutex mutex_;
    T data_;
};

template<class T>
struct LockGuardPtr
{
    T& operator*()
    {
        return m_ptr;
    }

    T* operator->()
    {
        return &m_ptr;
    }

    T* get()
    {
        return &m_ptr;
    }

    LockGuardPtr(LockGuarded<T>& sync)
        :lock_(sync.mutex_),
        ref_(sync.data_)
    {
    }

private:
    std::unique_lock<std::mutex> lock_;
    T& ref_;
};

