//
// Created by danie on 29-May-20.
//

#ifndef OS_WET3_THREADSAFELIST_H
#define OS_WET3_THREADSAFELIST_H

// put here list of spinlocks for the nodes in the list

<template class T>
class Node {

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
