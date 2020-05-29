#ifndef OS3_THREADSAFELIST_H
#define OS3_THREADSAFELIST_H

#include <pthread.h>
#include <iostream>
#include <iomanip> // std::setw

using namespace std;

template <class T>
class ThreadSafeList {
private:
    class Node {
        T data; // **unique**
        // spinlock
    };

    pthread_mutex_t list_mutex;
    Node* head;
    pthread_mutex_t size_mutex;
    int size;

public:
    List() {
        //initialize empty list
        // intizlie list_mutex and size_mutex
        // intialize dummy node
        // size = 0

        // if failed print to std:cerr  "<function name>:failed" and exit(-1)
    }
    bool insert(const T& data) {
        // hand in hand locking traversal:
        //      lock()
        //      if there is a node with the same data:  unlock and return false
        //      else {
        //          insert (in **ascending order**)
        //           if insert succeeded:
        //                 // lock size_mutex
        //                 size++
        //                 // unlock size_mutex
        //                 insert_test_hook
        //           else: unlock and return false
        //      }
        //      unlock()

        return true;
    }
    bool remove(const T& value) {
        // hand in hand locking traversal:
        //      lock()
        //      remove
        //      if remove succeeded:
        //          // lock size_mutex
        //          size--
        //          // unlock size_mutex
        //          remove_test_hook
        //          unlock
        //          return true
        //      unlock()

        return false;
    }

    unsigned int getSize() {
        int retVal = 0;
        // lock size_mutex
        // retVal = size
        // unlock size_mutex
        return retVal;
    }

    ~List() {
        // destroy empty list
        // destroy list_mutex and size_mutex
        // destroy dummy node
    }
    void print() {
        // todo: modify to ignore dummy node!

          pthread_mutex_lock(&list_mutex);
          Node* temp = head;
          if (temp == NULL)
          {
            cout << "";
          }
          else if (temp->next == NULL)
          {
            cout << temp->data;
          }
          else
          {
            while (temp != NULL)
            {
              cout << right << setw(3) << temp->data;
              temp = temp->next;
              cout << " ";
            }
          }
          cout << endl;
          pthread_mutex_unlock(&list_mutex);
        }

    virtual void __insert_test_hook();
    virtual void __remove_test_hook();


};


#endif //OS_WET3_THREADSAFELIST_H
