#include "Semaphore.hpp"


Semaphore::Semaphore() : Semaphore(0) {}

Semaphore::Semaphore(unsigned val) {
	counter =  val;
	// initilaize the condition variable
	pthread_cond_init(&_cond, NULL);
	// initilaize the mutex variable
	pthread_mutexattr_settype(&_attr, PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutex_init(&_mutex, &_attr);
}

void Semaphore::up() {
	pthread_mutex_lock(&_mutex);
	counter++;
	pthread_cond_signal(&_cond);
	//pthread_cond_broadcast(&(this->_cond)); ???
	pthread_mutex_unlock(&_mutex);
}

void Semaphore::down() {
	pthread_mutex_lock(&_mutex);
	// condition wait for counter to be greater than zero
	while (counter == 0) {
		pthread_cond_wait(&_cond, &_mutex);
	}
	// now counter is greater than zero and we can proceed
	counter--;
	pthread_mutex_unlock(&_mutex);
}
