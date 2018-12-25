#ifndef _QUEUEL_H
#define _QUEUEL_H
#include "Headers.hpp"
#include "Semaphore.hpp"

// Single Producer - Multiple Consumer queue
template <typename T>
class PCQueue {

public:
	PCQueue() : _sem_producer(1), produser_inside(0) {
		pthread_mutexattr_settype(&_c_attr, PTHREAD_MUTEX_ERRORCHECK);
		pthread_mutexattr_settype(&_p_attr, PTHREAD_MUTEX_ERRORCHECK);
		pthread_mutex_init(&_c_mutex, &_c_attr);
		pthread_mutex_init(&_p_mutex, &_p_attr);
	}
	
	// Blocks while queue is empty. When queue holds items, allows for a single
	// thread to enter and remove an item from the front of the queue and return it. 
	// Assumes multiple consumers.
	T pop() {
		pthread_mutex_lock(&_c_mutex);
		while (produser_inside);
		_sem_items.down();
		T item = _items.front();
		_items.pop();
		pthread_mutex_unlock(&_c_mutex);
		return item;
	} 

	// Allows for producer to enter with *minimal delay* and push items to back of the queue.
	// Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.  
	// Assumes single producer
	void push(const T& item) {
		pthread_mutex_lock(&_p_mutex);
		produser_inside = 1;
		_sem_producer.down();
		_items.push(item);
		_sem_items.up();
		_sem_producer.up();
		produser_inside = 0;
		pthread_mutex_unlock(&_p_mutex);
		return;
	}


private:
	// Add your class memebers here
	Semaphore _sem_producer; // sem with initial 1
	Semaphore _sem_items; // sem with initial 0
	pthread_mutex_t _c_mutex;
	pthread_mutex_t _p_mutex;
	pthread_mutexattr_t _c_attr;
	pthread_mutexattr_t _p_attr;
	int produser_inside;
	std::queue<T> _items;
};
// Recommendation: Use the implementation of the std::queue for this exercise
#endif