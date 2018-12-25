
#include "Semaphore.hpp"

Semaphore a(2);

void* workload(void* info) {
	a.down();
	int* arg = (int*)info;
	int i = 10;
	while(i!=0) {
		printf("%d running...\n",*arg);
		i--;
	}
	a.up();
	return NULL;
}
int main() {
	pthread_t t1,t2,t3,t4,t5;
	int aa = 1;
	int bb = 2;
	int cc = 3;
	int dd = 4;
	int ee = 5;
	pthread_create(&t1,NULL,workload,(void*)(&aa));
	pthread_create(&t2,NULL,workload,(void*)(&bb));
	pthread_create(&t3,NULL,workload,(void*)(&cc));
	pthread_create(&t4,NULL,workload,(void*)(&dd));
	pthread_create(&t5,NULL,workload,(void*)(&ee));
	
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	pthread_join(t3,NULL);
	pthread_join(t4,NULL);
	pthread_join(t5,NULL);
	
	return 0;
}