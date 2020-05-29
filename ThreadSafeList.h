#ifndef OS3_THREADSAFELIST_H
#define OS3_THREADSAFELIST_H

#include <pthread.h>
#include <iostream>
#include <iomanip> // std::setw

using namespace std;

template <class T>
class List {
private:
    class Node {
    public:
        Node(const T& data) : data(data),
                              next(nullptr)  {
            pthread_mutex_init(&mutex, NULL);
        }
        ~Node() {
            pthread_mutex_destroy(&mutex);
        }

        T data; // **unique**
        Node* next;
        pthread_mutex_t mutex;
    };


    pthread_mutex_t list_mutex, size_mutex;
    Node* head; // head points to dummy
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
        // hand over hand locking traversal
        pthread_mutex_lock(&(head->mutex)); // lock dummy

        Node* iter=head;
        while (iter->next) {
            pthread_mutex_lock(&(iter->next->mutex)); // lock the next node to check

            if (iter->next->data == value) { // found it!
                Node* to_delete = iter->next;
                iter->next = iter->next->next; // remove from list

                __remove_test_hook(); // test hook

                // delete the node (it's ok to unlock the lock and destroy the lock safely after
                // because we know for sure that no other thread would try to lock it in between)
                pthread_mutex_unlock(&(to_delete->mutex));
                delete to_delete; // mutex destroyed in Node's d'tor

                pthread_mutex_unlock(&(iter->mutex));

                // update size
                pthread_mutex_lock(&size_mutex);
                size--;
                pthread_mutex_unlock(&size_mutex);

                return true; // success
            }

            Node* prev = iter;
            iter=iter->next;
            pthread_mutex_unlock(&(prev->mutex));
        }

        pthread_mutex_unlock(&(iter->mutex)); // unlock
        return false;
    }

    unsigned int getSize() {
        int retVal = 0;
        pthread_mutex_lock(&size_mutex);
        retVal = size;
        pthread_mutex_unlock(&size_mutex);
        return retVal;
    }

    ~List() {
        // TODO: i think we don't need here mutual exclusion

        // destroy all nodes (includes dummy)
        Node* iter = head;
        while (iter) {
            Node* to_delete = iter;
            iter=iter->next;
            delete to_delete; // d'tor will destroy node's mutex
        }

        // destroy list_mutex and size_mutex
        pthread_mutex_destroy(&list_mutex);
        pthread_mutex_destroy(&size_mutex);
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
