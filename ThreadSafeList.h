#ifndef THREAD_SAFE_LIST_H_
#define THREAD_SAFE_LIST_H_

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
            if (pthread_mutex_init(&mutex, NULL) != 0) {
                std::cerr << "pthread_mutex_init:failed" << std::endl;
            }
        }
        ~Node() {
            if (pthread_mutex_destroy(&mutex) != 0) {
                std::cerr << "pthread_mutex_destroy:failed" << std::endl;
            }
        }
    };

    // List field members
    int size;
    Node* head; // head points to dummy
    pthread_mutex_t size_mutex;

    void ReleaseLocks(Node* prev, Node* current) {
        if (current) {
            if (pthread_mutex_unlock(&(current->mutex)) != 0) {
                std::cerr << "pthread_mutex_unlock:failed" << std::endl;
            }
        }
        if (pthread_mutex_unlock(&(prev->mutex)) != 0) {
            std::cerr << "pthread_mutex_unlock:failed" << std::endl;
        }
    }

public:
    /**
     * Constructor
     */
    List() : size(0) {
        //initialize empty list with dummy nodes
        try {
            head = new Node(T());
        } catch (std::bad_alloc&) { // allocation error
            std::cerr << "new:failed" << std::endl;
            exit(-1);
        }
        // initialize the size lock
        if (pthread_mutex_init(&size_mutex, NULL) != 0) {
            std::cerr << "pthread_mutex_init:failed" << std::endl;
        }
    }

    /**
     * Destructor
     */
    ~List() {
        // destroy all nodes (includes dummy)
        Node* iter = head;
        while (iter) {
            Node* to_delete = iter;
            iter=iter->next;
            delete to_delete; // d'tor will destroy node's mutex
        }

        // destroy size_mutex
        if (pthread_mutex_destroy(&size_mutex) != 0) {
            std::cerr << "pthread_mutex_destroy:failed" << std::endl;
        }
    }

    /**
     * Insert new node to list while keeping the list ordered in an ascending order
     * If there is already a node has the same data as @param data then return false (without adding it again)
     * @param data the new data to be added to the list
     * @return true if a new node was added and false otherwise
     */
    bool insert(const T& data) {
        // hand over hand locking traversal
        if (pthread_mutex_lock(&(head->mutex)) != 0) { // lock first node (dummy)
            std::cerr << "pthread_mutex_lock:failed" << std::endl;
            return false;
        }

        Node* prev = head;
        Node* current = head->next;
        while (current) {
            if (pthread_mutex_lock(&(current->mutex)) != 0) { // lock the next node to check
                std::cerr << "pthread_mutex_lock:failed" << std::endl;
                pthread_mutex_unlock(&(prev->mutex));
                return false;
            }

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

            if (pthread_mutex_unlock(&(prev->mutex)) != 0) { // unlock the previous node
                std::cerr << "pthread_mutex_unlock:failed" << std::endl;
            }

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
            std::cerr << "new:failed" << std::endl;
            ReleaseLocks(prev, current); // release the locks
            // (if reached the end of the list current is null so no need to unlock)

            return false;                // return failure
        }

        // update size
        if (pthread_mutex_lock(&size_mutex) != 0) {
            std::cerr << "pthread_mutex_lock:failed" << std::endl;
        }
        size++;
        if (pthread_mutex_unlock(&size_mutex) != 0) {
            std::cerr << "pthread_mutex_unlock:failed" << std::endl;
        }

        __insert_test_hook(); // test hook

        ReleaseLocks(prev, current); // release the locks
        // (if reached the end of the list current is null and isn't unlocked)

        return true; // return success
    }

    /**
     * Remove the node that its data equals to @param value
     * @param value the data to lookup a node that has the same data to be removed
     * @return true if a matched node was found and removed and false otherwise
     */
    bool remove(const T& value) {
        // hand over hand locking traversal
        if (pthread_mutex_lock(&(head->mutex)) != 0){ // lock dummy
            std::cerr << "pthread_mutex_lock:failed" << std::endl;
            return false;
        }

        Node* iter=head;
        while (iter->next) {
            if (pthread_mutex_lock(&(iter->next->mutex)) != 0) {  // lock the next node to check
                std::cerr << "pthread_mutex_lock:failed" << std::endl;
                pthread_mutex_unlock(&(iter->mutex));
                return false;
            }

            if (iter->next->data == value) { // found it!
                Node* to_delete = iter->next;
                iter->next = iter->next->next; // remove from list

                // update size
                if (pthread_mutex_lock(&size_mutex) != 0) {
                    std::cerr << "pthread_mutex_lock:failed" << std::endl;
                }
                size--;
                if (pthread_mutex_unlock(&size_mutex) != 0) {
                    std::cerr << "pthread_mutex_unlock:failed" << std::endl;
                }

                __remove_test_hook(); // test hook

                // delete the node (it's ok to unlock the lock and destroy the lock safely after
                // because we know for sure that no other thread would try to lock it in between)
                if (pthread_mutex_unlock(&(to_delete->mutex)) != 0) {
                    std::cerr << "pthread_unmutex_lock:failed" << std::endl;
                }
                delete to_delete; // mutex destroyed in Node's d'tor

                if (pthread_mutex_unlock(&(iter->mutex)) != 0) {
                    std::cerr << "pthread_mutex_unlock:failed" << std::endl;
                }

                return true; // success
            }

            Node* prev = iter;
            iter=iter->next;
            if (pthread_mutex_unlock(&(prev->mutex)) != 0) {
                std::cerr << "pthread_mutex_unlock:failed" << std::endl;
            }
        }

        if (pthread_mutex_unlock(&(iter->mutex)) != 0) {
            std::cerr << "pthread_mutex_unlock:failed" << std::endl;
        } // unlock
        return false;
    }

    /**
     * Returns the current size of the list
     * @return current size of the list
     */
    unsigned int getSize() {
        int retVal = 0;
        if (pthread_mutex_lock(&size_mutex) != 0) {
            std::cerr << "pthread_mutex_lock:failed" << std::endl;
        }
        retVal = size;
        if (pthread_mutex_unlock(&size_mutex) != 0) {
            std::cerr << "pthread_mutex_unlock:failed" << std::endl;
        }
        return retVal;
    }

    // Don't remove
    void print() {
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
        }

    // Don't remove
    virtual void __insert_test_hook() {}
    // Don't remove
    virtual void __remove_test_hook() {}
};

#endif //THREAD_SAFE_LIST_H_
