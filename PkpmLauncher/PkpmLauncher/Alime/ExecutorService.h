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
        void setMaxQueueSize(int maxSize);
        void setThreadInitCallback(const Task& cb);
        void start(int numThreads);
        void stop();
        const std::string& name() const;
        size_t queueSize() const;
        void run(Task f);
    private:
        bool isFull() const;
        void runInThread();
        Task take();
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
