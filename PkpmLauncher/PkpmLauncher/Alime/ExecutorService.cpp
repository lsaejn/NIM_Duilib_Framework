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
            stop();
        }
    }

    void ExecutorService::start(int numThreads)
    {
        assert(threads_.empty());
        running_ = true;
        threads_.reserve(numThreads);
        for (int i = 0; i < numThreads; ++i)
        {
            char id[32];
            snprintf(id, sizeof id, "%d", i + 1);
            threads_.emplace_back(new std::thread(
                std::bind(&ExecutorService::runInThread, this), name_ + id));
            //threads_[i]->start();
        }
        if (numThreads == 0 && threadInitCallback_)
        {
            threadInitCallback_();
        }
    }

    void ExecutorService::stop()
    {
        {
            std::lock_guard lock(mutex_);
            running_ = false;
            notEmpty_.notify_all();
        }
        for (auto& thr : threads_)
        {
            thr->join();
        }
    }

    void ExecutorService::run(Task task)
    {
        if (threads_.empty())
        {
            task();
        }
        else
        {
            std::unique_lock lock(mutex_);
            while (isFull())
            {
                notFull_.wait(lock);
            }
            assert(!isFull());

            queue_.push_back(std::move(task));
            notEmpty_.notify_one();
        }
    }

    void ExecutorService::setMaxQueueSize(int maxSize)
    { 
        maxQueueSize_ = maxSize;
    }

    void ExecutorService::setThreadInitCallback(const Task& cb)
    {
        threadInitCallback_ = cb;
    }

    const std::string& ExecutorService::name() const
    {
        return name_;
    }

    ExecutorService::Task ExecutorService::take()
    {
        std::unique_lock lock(mutex_);
        // always use a while-loop, due to spurious wakeup
        while (queue_.empty() && running_)
        {
            notEmpty_.wait(lock);
        }
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

    bool ExecutorService::isFull() const
    {
        //mutex_.assertLocked();
        return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
    }

    void ExecutorService::runInThread()
    {
        try
        {
            if (threadInitCallback_)
            {
                threadInitCallback_();
            }
            while (running_)
            {
                Task task(take());
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

    size_t ExecutorService::queueSize() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
}