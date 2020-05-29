#include "Barrier.h"

// declare global semaphore and mutex

Barrier::Barrier(unsigned int num_of_thread) {
    // sem_init( 0 )
    // mutex intialize
    // max_threads = num_of_thread
}

void Barrier::Wait() {
    // mutex lock
    // count++
    // if count = max_threads:
    //      do sem_post n-1 times
    //      n = 0
    // mutex unlock
    // sem_wait
}

Barrier::~Barrier() {
    // sem_destroy
    // mutex destroy
}