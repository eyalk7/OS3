//
// Created by danie on 29-May-20.
//

#ifndef OS_WET3_BARRIER_H
#define OS_WET3_BARRIER_H


class Barrier {

public:
    Barrier(unsigned int num_of_thread);
    void Wait();
    ~Barrier();

private:

};


#endif //OS_WET3_BARRIER_H
