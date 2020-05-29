#ifndef OS3_BARRIER_H
#define OS3_BARRIER_H

#include <pthread.h>
#include <semaphore.h>

class Barrier {
private:
    int num_of_threads, threads_in_barrier;
    sem_t sem;
    pthread_mutex_t mutex;
public:
    Barrier(unsigned int num_of_thread);
    void Wait();
    ~Barrier();
};


#endif //OS_WET3_BARRIER_H
