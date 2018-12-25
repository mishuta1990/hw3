#include <iostream>
#include "PCQueue.hpp"

#include "Semaphore.hpp"
#define N 10
Semaphore s(10);
int x=0;
PCQueue<int>* q = new PCQueue<int>;

void* print_msg(void* i){
    s.down();
    x++;
    for (long j = 0; j <100000000 ; ++j) {
    }
    if(x > 10)
        cout << x << endl;
    /*cout << *(int*)i << " and x = " << x << endl;*/
    x--;
    s.up();
    return nullptr;
}

void* writer(void* data) {
    q->push(*(int*)data);
    cout << "I pushed something  " << *(int*)data << endl;
    return nullptr;
}

void* reader(void* nothing) {
    int res = q->pop();
    cout << res << endl;
    return nothing;
}

int main() {
    pthread_t a[N];
/*    int id[N];
    for (int k = 0; k <N ; ++k) {
        id[k]=k+1;
    }
    for (int i = 0; i < N; ++i) {
        pthread_create(&a[i], nullptr, print_msg, (void*)&id[i]);

    }
    for (int j = 0; j < N; ++j) {
        pthread_join(a[j], nullptr);
    }*/


    int data[N/2];
    for (int j = 0; j < N/2; ++j)
        data[j] = j;
    for (int i = 0; i <N ; i++) {
        pthread_create(&a[i++], nullptr, writer, (void*)&data[i/2]);
        pthread_create(&a[i], nullptr, reader, nullptr);
    }

    for (int j = 0; j < N; ++j) {
        pthread_join(a[j], nullptr);
    }



    return 0;
}