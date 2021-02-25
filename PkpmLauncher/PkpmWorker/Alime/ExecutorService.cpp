#include "pch.h"
#include "ExecutorService.h"

namespace Alime 
{
    ExecutorService::ExecutorService(const std::string_view nameArg)
        : mutex_(),
        notEmpty_(),
        notFull_(),
        name_(nameArg),
        maxQueueSize_(0),
        running_(false)
    {
    }

    ExecutorService::~ExecutorService()
    {
        if (running_)
        {
            Stop();
        }
    }

    void ExecutorService::Start(int numThreads)
    {
        assert(threads_.empty());
        running_ = true;
        threads_.reserve(numThreads);
        for (int i = 0; i < numThreads; ++i)
        {
            char id[32] = { 0 };
            snprintf(id, sizeof id, "%d", i + 1);
            threads_.emplace_back(new std::thread(
                std::bind(&ExecutorService::RunInThread, this), name_ + id));
        }
        if (numThreads == 0 && threadInitCallback_)
        {
            threadInitCallback_();
        }
    }

    void ExecutorService::Stop()
    {
        {
            std::lock_guard lock(mutex_);
            running_ = false;
            notEmpty_.notify_all();
            notFull_.notify_all();
        }
        for (auto& thr : threads_)
        {
            thr->join();
        }
    }

    //这里有个潜在的bug，若是当前队列已满
    //那么非本线程的生产者将阻塞在本函数。
    //此时线程池执行stop，程序将永远阻塞。所以我在stop里唤醒了所有等待中的线程
    void ExecutorService::Run(Task task)
    {
        if (threads_.empty())
        {
            task();
        }
        else
        {
            std::unique_lock lock(mutex_);
            while (IsFull()&& running_)
            {
                notFull_.wait(lock);
            }
            if (!running_)
                return;
            assert(!IsFull());

            queue_.push_back(std::move(task));
            notEmpty_.notify_one();
        }
    }

    void ExecutorService::SetMaxQueueSize(int maxSize)
    { 
        maxQueueSize_ = maxSize;
    }

    void ExecutorService::SetThreadInitCallback(const Task& cb)
    {
        threadInitCallback_ = cb;
    }

    const std::string& ExecutorService::GetName() const
    {
        return name_;
    }

    ExecutorService::Task ExecutorService::Take()
    {
        std::unique_lock lock(mutex_);
        notEmpty_.wait(lock, [&]() {
            return !(queue_.empty() && running_);
            });
        Task task;
        if (!queue_.empty())
        {
            task = queue_.front();
            queue_.pop_front();
            if (maxQueueSize_ > 0)
            {
                notFull_.notify_one();
            }
        }
        return task;
    }

    bool ExecutorService::IsFull() const
    {
        return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
    }

    void ExecutorService::RunInThread()
    {
        try
        {
            if (threadInitCallback_)
            {
                threadInitCallback_();
            }
            while (running_)
            {
                Task task(Take());
                if (task)
                {
                    task();
                }
            }
        }
        catch (const std::exception & ex)
        {
            fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            abort();
        }
        catch (...)
        {
            fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
            throw; // rethrow
        }
    }

    //or, use an atomic var instead
    size_t ExecutorService::QueueSize() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
}