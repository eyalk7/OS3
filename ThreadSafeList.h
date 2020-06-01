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
        // Node field members
        T data; // **unique**
        Node* next;
        pthread_mutex_t mutex;

        // Node functions
        explicit Node(const T& data) : data(data),
                              next(nullptr)  {
            pthread_mutex_init(&mutex, NULL);
        }
        ~Node() {
            pthread_mutex_destroy(&mutex);
        }
    };

    // List field members
    int size;
    Node* head; // head points to dummy
    pthread_mutex_t list_mutex, size_mutex;

    void ReleaseLocks(Node* prev, Node* current) {
        pthread_mutex_unlock(&(prev->mutex));
        if (current) {
            pthread_mutex_unlock(&(current->mutex));
        }
    }

public:
    // List functions
    List() : size(0) {
        //initialize empty list with dummy nodes
        try {
            head = new Node(T());
        } catch (std::bad_alloc&) { // allocation error
            std::cerr << "List():failed";
            exit(-1);
        }
        // initialize the locks
        int retVal1 = pthread_mutex_init(&list_mutex, NULL);
        int retVal2 = pthread_mutex_init(&size_mutex, NULL);

        // check if mutex initialization failed
        if (retVal1 != 0 || retVal2 != 0) {
            // TODO: is this check necessary ?
            std::cerr << "List():failed";
            exit(-1);
        }
    }
    bool insert(const T& data) {
        Node* prev = head;
        Node* current = head->next;

        // hand over hand locking traversal
        pthread_mutex_lock(&(prev->mutex)); // lock first node (dummy)
        while (current) {
            pthread_mutex_lock(&(current->mutex)); // lock the next node to check

            // check if the node already exists
            if (current->data == data) {
                ReleaseLocks(prev, current); // release the locks
                return false;                // return failure
            }

            // check if to insert here
            if (data < current->data) {
                break; // found insertion place
            }
            // else, keep traversing the list looking for appropriate insertion place

            pthread_mutex_unlock(&(prev->mutex)); // unlock the previous node

            // update pointers for next iteration
            prev = current;
            current = current->next;
        }

        // loop was broken OR reached the end of the list
        // (either way, found insertion place)

        try {
            // insert new node
            Node* to_insert = new Node(data);
            prev->next = to_insert;
            to_insert->next = current;

        } catch (std::bad_alloc&) {      // on allocation error
            ReleaseLocks(prev, current); // release the locks
            // (if reached the end of the list current is null so no need to unlock)

            return false;                // return failure
        }

        __insert_test_hook(); // test hook

        ReleaseLocks(prev, current); // release the locks
        // (if reached the end of the list current is null and isn't unlocked)

        // update size and return success
        pthread_mutex_lock(&size_mutex);
        size++;
        pthread_mutex_unlock(&size_mutex);
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
          pthread_mutex_lock(&list_mutex);
          Node* temp = head->next;  // skip dummy
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

    // Don't remove
    virtual void __insert_test_hook() {}
    // Don't remove
    virtual void __remove_test_hook() {}
};

#endif //OS_WET3_THREADSAFELIST_H
