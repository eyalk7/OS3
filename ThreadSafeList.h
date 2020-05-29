#ifndef OS3_THREADSAFELIST_H
#define OS3_THREADSAFELIST_H

// put here list of spinlocks for the nodes in the list

<template class T>
class Node {
    int index;
    int data; // **unique**
};

<template class T>
class ThreadSafeList {


public:
    List();
    bool insert(const T& data);
    bool remove(const T& value);
    unsigned int getSize();
    ~List();
    void print();

    virtual void __insert_test_hook();
    virtual void __remove_test_hook();

private:

};


#endif //OS_WET3_THREADSAFELIST_H
