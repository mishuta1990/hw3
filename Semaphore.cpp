#include "Semaphore.hpp"


Semaphore::Semaphore() {
	counter = 0;
	// initilaize the condition variable
	pthread_cond_init(&(this->_cond),NULL);
	// initilaize the mutex variable
	_mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER;
	//pthread_mutex_init(&(this->_mutex), &attr);
}

Semaphore::Semaphore(unsigned val) {
	counter =  val;
	// initilaize the condition variable
	pthread_cond_init(&(this->_cond), NULL);
	// initilaize the mutex variable
	_mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER;
}

void Semaphore::up() {
	pthread_mutex_lock(&(this->_mutex));
	this->counter++;
	pthread_cond_signal(&(this->_cond));
	//pthread_cond_broadcast(&(this->_cond)); ???
	pthread_mutex_unlock(&(this->_mutex));
}

void Semaphore::down() {
	pthread_mutex_lock(&(this->_mutex));
	// condition wait for counter to be greater than zero
	while (this->counter == 0) {
		pthread_cond_wait(&(this->_cond), &(this->_mutex));
	}
	// now counter is greater than zero and we can proceed
	this->counter--;
	pthread_mutex_unlock(&(this->_mutex));
}
