#pragma once
#include "Alime/NonCopyable.h"
#include <functional>
#include <string>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Alime
{
	class ExecutorService:public noncopyable
	{
	public:
		typedef std::function<void()> Task;
        explicit ExecutorService(const std::string_view nameArg = "ExecutorService");
        ~ExecutorService();
        void SetMaxQueueSize(int maxSize);
        void SetThreadInitCallback(const Task& cb);
        void Start(int numThreads);
        void Stop();
        const std::string& GetName() const;
        size_t QueueSize() const;
        void Run(Task f);
    private:
        bool IsFull() const;
        void RunInThread();
        Task Take();
    private:
        std::condition_variable notEmpty_ ;
        std::condition_variable notFull_ ;                  
        std::vector<std::unique_ptr<std::thread>> threads_;
        std::deque<Task> queue_;
        std::string name_;
        mutable std::mutex mutex_;
        Task threadInitCallback_;
        size_t maxQueueSize_;
        bool running_;
	};
}
