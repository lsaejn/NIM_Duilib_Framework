#pragma once
#include <mutex>

/*
struct threadData{
void fuck(){...}
}
SafeAccess<threadData> d;
d.lock()->fuck();
*/

template<class T>
struct SafeAccessPtr;

template<class T>
struct SafeAccess
{
    friend struct SafeAccessPtr<T>;
    SafeAccessPtr<T> lock()
    {
        return *this;
    }

private:
    std::mutex mutex_;
    T data_;
};

template<class T>
struct SafeAccessPtr
{
    T& operator*()
    {
        return ref_;
    }

    T* operator->()
    {
        return &ref_;
    }

    T* get()
    {
        return &ref_;
    }

    SafeAccessPtr(SafeAccess<T>& sync)
        :lock_(sync.mutex_),
        ref_(sync.data_)
    {
    }

private:
    std::unique_lock<std::mutex> lock_;
    T& ref_;
};

