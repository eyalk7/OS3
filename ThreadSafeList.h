//
// Created by danie on 29-May-20.
//

#ifndef OS3_THREADSAFELIST_H
#define OS3_THREADSAFELIST_H

#include <pthread.h>
#include <iostream>
#include <iomanip> // std::setw

using namespace std;
// put here list of spinlocks for the nodes in the list

template <class T>
class ThreadSafeList {


public:
    List();
    bool insert(const T& data);
    bool remove(const T& value);
    unsigned int getSize();
    ~List();
    void print() {
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

    class Node {

    };

private:
    pthread_mutex_t list_mutex;
    Node* head;
};


#endif //OS_WET3_THREADSAFELIST_H
