#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H
#include "Headers.hpp"

// Synchronization Warm up 
class Semaphore {
public:
	Semaphore(); // Constructs a new semaphore with a counter of 0
	Semaphore(unsigned val); // Constructs a new semaphore with a counter of val


	void up(); // Mark: 1 Thread has left the critical section - sem_post
	void down(); // Block until counter >0, and mark - One thread has entered the critical section. - sem_wait

private:
	int counter;
	pthread_mutex_t _mutex;
	pthread_mutexattr_t _attr;
	pthread_cond_t _cond;
};

#endif
