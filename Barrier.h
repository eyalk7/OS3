#ifndef OS3_BARRIER_H
#define OS3_BARRIER_H

#include <pthread.h>
#include <semaphore.h>

class Barrier {
private:
    int num_of_threads, threads_in_barrier;
    sem_t sem, fence;
    pthread_mutex_t mutex;
public:
    explicit Barrier(unsigned int num_of_thread);
    void wait();
    ~Barrier();

    // function for testing
    //int waitingThreads();
};


#endif //OS_WET3_BARRIER_H
