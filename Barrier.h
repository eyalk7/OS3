#ifndef OS3_BARRIER_H
#define OS3_BARRIER_H


class Barrier {

public:
    Barrier(unsigned int num_of_thread);
    void Wait();
    ~Barrier();

private:

};


#endif //OS_WET3_BARRIER_H
