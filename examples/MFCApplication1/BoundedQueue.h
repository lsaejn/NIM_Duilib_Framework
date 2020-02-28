#pragma once
#ifndef BOUNDEDQUEUE_H_
#define BOUNDEDQUEUE_H_
#include <deque>
#include <type_traits>
//

class Error
{
private:
	const char* message;
public:
	Error(const char* ms)
	{
		message=ms;
	}
	const char* what() const
	{
		return message;
	}
};

#define CHECK_ERROR(CONDITION, MESSAGE_STRING) do{if(!(CONDITION))throw Error(MESSAGE_STRING);}while(0)
namespace collection_utility
{
	/*
		这群人太厉害了，活活把一个有限队列改成了vector
	*/
	template<typename T, typename VecTy=std::deque<T>>
	class BoundedQueue
	{
	public:
		explicit BoundedQueue(const int maxSize)
			:queue_(0), 
			maxSize_(maxSize)
		{
		}

		template<typename Iter>
		void put(const Iter start, const Iter end)
		{
			//static_assert(std::is_same_v<Iter::iterator_category, random_access_iterator_tag>, "bad param");
			for (auto It_copy = start; It_copy != end; ++It_copy)
				put(*It_copy);
		}

		void put(const T& x)
		{
			if (full())
				pop();
			queue_.push_front(x);			
		}

		void pop()
		{
			if (empty())
				throw("logic error");
			queue_.pop_back();
		}

		bool empty() const
		{
			return queue_.empty();
		}

		bool full() const
		{
			return queue_.size()== maxSize_;
		}

		size_t size() const
		{
			return queue_.size();
		}

		T& operator[](const int index)
		{
			rangeNoError(index);
			return queue_[index];
		}

		typename VecTy::const_iterator begin()
		{
			return queue_.cbegin();
		}

		typename VecTy::const_iterator end()
		{
			return queue_.cend();
		}

		void clear()
		{
			queue_.clear();
		}

		/*
		没有工程时，前端传了0过来....
		这直接导致不必要的检查
		*/
		void moveToFront(const int index)
		{
			rangeNoError(index);
			T copy = queue_[index];
			for (int i = index; i > 0; --i)
				queue_[i] = queue_[i - 1];
			queue_[0] = copy;
		}

		void deleteAt(const int index)
		{
			rangeNoError(index);
			if((queue_.size()-1)==index)
				queue_.pop_back();
			else
				remove(index);
		}

	private:
		void rangeNoError(const int index)
		{

			CHECK_ERROR( (index>=0) && (static_cast<size_t>(index)<queue_.size()),"index not in range.");
		}

		//replace by deleteAt
		void remove(const int index)
		{
			for (size_t i = index; i < queue_.size() - 1;++i)
				queue_[i] = queue_[i+1];
			queue_.pop_back();
		}

	private:
		VecTy	queue_;
		size_t maxSize_;
	};
}

/*
TEST(BoundedQueue,ALL)
{
	collection_utility::BoundedQueue<int> q(5);
	for(int i=0;i!=5;++i)
		q.put(i);
	q.outPut(std::cout);
	q.deleteAt(2);
	q.outPut(std::cout);
	q.moveToFront(2);
	q.outPut(std::cout);
	q.moveToFront(2);
	q.outPut(std::cout);
	try{
		q.moveToFront(-1);
	}catch (Error& e){
		printf("%s\n",e.what());
		q.outPut(std::cout);
	}
	q.clear();
test:
	int i=-1;
	try{
		for(;i!=2;++i)
			q.moveToFront(0);
	}catch(Error& e){
		printf("%s\n",e.what());
		q.outPut(std::cout);
		goto:test;
	}
}
*/
#endif
