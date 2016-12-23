#ifndef __H_ATOMIC_LIST
#define __H_ATOMIC_LIST

#include <thread>
#include <mutex>
#include <forward_list>


template<typename T>
struct list_elements
{
	std::mutex mtx;
	
  std::forward_list<T> head;

	list_elements() {}

	void push(const T data)
  {
    	mtx.lock();

      head.push_front(data);

    	mtx.unlock();
  }

    void clear()
    {
    	head.clear();
    }

    size_t size()
    {
        return head.size();
    }
};

#endif