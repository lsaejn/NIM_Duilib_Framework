#pragma once
#include <mutex>

template<class T>
struct LockGuardPtr;

template<class T>
struct LockGuarded
{
    friend struct LockGuardPtr<T>;

    LockGuardPtr<T> lock() { return *this; }

private:
    std::mutex m_mutex;
    T m_t;
};

template<class T>
struct LockGuardPtr
{
    T& operator*() { return m_ptr; }
    T* operator->() { return &m_ptr; }

    T* get() { return &m_ptr; }

    LockGuardPtr(LockGuarded<T>& sync) : m_lock(sync.m_mutex), m_ptr(sync.m_t) {}

private:
    std::unique_lock<std::mutex> m_lock;
    T& m_ptr;
};
