#ifndef __H_ATOMIC_LIST
#define __H_ATOMIC_LIST

#include <thread>
#include <mutex>

template<typename T>
struct node
{
    T data;
    node* next;
    node(const T& data) : data(data), next(nullptr) {}
};

template<typename T>
struct list_elements
{
	std::mutex mtx;
	
	int count_elements;
	
	node<T> *head;
	node<T> *tail;

	list_elements() : count_elements(0), head(nullptr), tail(nullptr) {}

	int push(const T data)
    {
      	node<T> *new_node = new node<T>(data);

      	int elem_pos;
      	
      	mtx.lock();

      	if(tail != nullptr)
      	{
      		tail->next = new_node;
      		tail = new_node;
      	}
      	else
      	{
      		tail = new_node;
      		head = new_node;
      	}

      	elem_pos = count_elements++;
      	
      	mtx.unlock();

      	return elem_pos;
    }

    void clear()
    {
    	node<T> *temp_curr = head, *temp_next;

    	mtx.lock();

    	while(temp_curr != nullptr)
    	{
    		temp_next = temp_curr->next;
    		delete temp_curr;
    		temp_curr = temp_next;
    	}

    	head = nullptr;
    	tail = nullptr;

    	mtx.unlock();
    }
};

#endif