#ifndef BARRIER_H
#define BARRIER_H

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


#endif // BARRIER_H
