#ifndef _QUEUEL_H
#define _QUEUEL_H

#include "Headers.hpp"
#include "Semaphore.hpp"

// Single Producer - Multiple Consumer queue
template <typename T> 
class PCQueue {

public:
	PCQueue() {
		this->_sem_items = Semaphore();
		this->_sem_producer = Semaphore(1);
	}
	// Blocks while queue is empty. When queue holds items, allows for a single
	// thread to enter and remove an item from the front of the queue and return it. 
	// Assumes multiple consumers.
	// ==== this is like read ====
	T pop() {
		_sem_items.down();
		T item = _items.front();
		_items.pop();
		return item;
	}

	// Allows for producer to enter with *minimal delay* and push items to back of the queue.
	// Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.  
	
	// Assumes single producer 
	// ==== this is like write ====
	void push(const T& item) {
		_sem_producer.down();
		_items.push(item);
		_sem_items.up();
		_sem_producer.up();
		return;
	}

private:

	
	Semaphore _sem_producer;
	Semaphore _sem_items; // sem with initial 0
	
	std::queue<T> _items;
	pthread_mutex_t _mutex;

	// Add your class memebers here
};
// Recommendation: Use the implementation of the std::queue for this exercise
#endif