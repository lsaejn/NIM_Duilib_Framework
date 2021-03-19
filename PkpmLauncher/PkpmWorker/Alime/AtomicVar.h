#pragma once

#include <intrin.h>
#include <stdint.h>

#include "NonCopyable.h"

namespace Alime::password {
    const char* enterpriseMail = "6284508Sxx..";
    const char* yunHWClouding = "6284508Sxx..";
    const char* fTransBox = "a123456";
    const char* fTransHWClouding = "6282881Sxx..";
    const char* oss = "Chj.In_3917";
    const char* jira = "gl=6282881";
}

namespace Alime::base
{
    template<typename T>
    class AtomicIntegerT : noncopyable
    {
    public:
        AtomicIntegerT()
            : value_(0)
        {
        }

        AtomicIntegerT(const AtomicIntegerT& that)
            : value_(that.get())
        {}

        AtomicIntegerT& operator=(const AtomicIntegerT& that)
        {
            getAndSet(that.get());
            return *this;
        }

        T get()
        {
#ifndef _MSC_VER
            return __sync_val_compare_and_swap(&value_, 0, 0);
#else
            return InterlockedExchangeAdd(&value_, 0);
#endif // #ifndef _MSC_VER
        }

        T getAndAdd(T x)
        {
#ifndef _MSC_VER
            return __sync_fetch_and_add(&value_, x);
#else
            T v = get();
            InterlockedExchangeAdd(&value_, 1);
            return v;
#endif // #ifndef _MSC_VER
        }

        T addAndGet(T x)
        {
            return getAndAdd(x) + x;
        }

        T incrementAndGet()
        {
            // or InterlockedIncrement(&value_) in win
            return addAndGet(1);
        }

        T decrementAndGet()
        {
            //or InterlockedDecrement(&value_) in win
            return addAndGet(-1);
        }

        void add(T x)
        {
            getAndAdd(x);
        }

        void increment()
        {
            incrementAndGet();
        }

        void decrement()
        {
            decrementAndGet();
        }

        T getAndSet(T newValue)
        {
            // in gcc >= 4.7: __atomic_exchange_n(&value, newValue, __ATOMIC_SEQ_CST)       
#ifndef _MSC_VER
            return __sync_lock_test_and_set(&value_, newValue);
#else
            return InterlockedExchangeAdd(&value_, T);
#endif // #ifndef _MSC_VER
        }

    private:
        volatile T value_;
    };

    using Atom32 = AtomicIntegerT<int32_t>;
    using Atom64 = AtomicIntegerT<int64_t>;
}
