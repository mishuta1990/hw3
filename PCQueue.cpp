/*#include "PCQueue.hpp"


template <class T>
PCQueue<T>::PCQueue() {
	this->_sem_items = Semaphore();
	this->_sem_producer = Semaphore(1);
}


template <class T>
T PCQueue<T>::pop() {
	_sem_items.down();
	T item = _items.front();
	_items.pop();
	return item;
}

template <class T>
void PCQueue<T>::push(const T& item){
	_sem_producer.down();
	_items.push(item);
	_sem_items.up();
	_sem_producer.up();
	return;
}
*/