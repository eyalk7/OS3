#include "Barrier.h"

Barrier::Barrier(unsigned int num_of_threads) : num_of_threads(num_of_threads),
                                                threads_in_barrier(0) {
    sem_init(&sem, 0, 0); // initialize semaphore
    pthread_mutex_init(&mutex, NULL); // initialize mutex
}

void Barrier::wait() {
    pthread_mutex_lock(&mutex); // lock mutex
    threads_in_barrier++;
    if (threads_in_barrier == num_of_threads) { // release all
        for (int i=0; i<num_of_threads; i++) sem_post(&sem);
        threads_in_barrier = 0;
    }
    pthread_mutex_unlock(&mutex); // unlock mutex

    // barrier
    sem_wait(&sem);
}

Barrier::~Barrier() {
    sem_destroy(&sem);
    pthread_mutex_destroy(&mutex);
}

// function for testing
/*
int Barrier::waitingThreads() {
    pthread_mutex_lock(&mutex); // lock mutex
    int retVal = threads_in_barrier;
    pthread_mutex_unlock(&mutex); // unlock mutex
    return retVal;
}
 */