#pragma once
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace Alime
{
	class CountDownLatch
	{
	public:
		CountDownLatch(int count)
			:count_(count),
			cv_(),
			lock_()
		{

		}
		void countDown()
		{
			// fix me, use std::lock_guard
			std::unique_lock<std::mutex> lg(lock_);
			--count_;
			if (0 == count_)
				cv_.notify_all();
		}

		void wait()
		{
			std::unique_lock<std::mutex> lg(lock_);
			while (count_ > 0)
			{
				cv_.wait(lg);
			}		
		}

		int getCount()
		{
			std::lock_guard guard(lock_);
			return count_;
		}

	private:
		int count_;
		std::condition_variable cv_;
		std::mutex lock_;
	};
}
