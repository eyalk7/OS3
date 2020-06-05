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
            pthread_mutex_init(&mutex, NULL);
        }
        ~Node() {
            pthread_mutex_destroy(&mutex);
        }
    };

    // List field members
    int size;
    Node* head; // head points to dummy
    pthread_mutex_t size_mutex;

    void ReleaseLocks(Node* prev, Node* current) {
        if (current) {
            pthread_mutex_unlock(&(current->mutex));
        }
        pthread_mutex_unlock(&(prev->mutex));
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
        pthread_mutex_init(&size_mutex, NULL);
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
        pthread_mutex_destroy(&size_mutex);
    }

    /**
     * Insert new node to list while keeping the list ordered in an ascending order
     * If there is already a node has the same data as @param data then return false (without adding it again)
     * @param data the new data to be added to the list
     * @return true if a new node was added and false otherwise
     */
    bool insert(const T& data) {
        // hand over hand locking traversal
        pthread_mutex_lock(&(head->mutex)); // lock first node (dummy)

        Node* prev = head;
        Node* current = head->next;
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

        // update size
        pthread_mutex_lock(&size_mutex);
        size++;
        pthread_mutex_unlock(&size_mutex);

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
        pthread_mutex_lock(&(head->mutex)); // lock dummy

        Node* iter=head;
        while (iter->next) {
            pthread_mutex_lock(&(iter->next->mutex)); // lock the next node to check

            if (iter->next->data == value) { // found it!
                Node* to_delete = iter->next;
                iter->next = iter->next->next; // remove from list

                // update size
                pthread_mutex_lock(&size_mutex);
                size--;
                pthread_mutex_unlock(&size_mutex);

                __remove_test_hook(); // test hook

                pthread_mutex_unlock(&(iter->mutex));

                // delete the node (it's ok to unlock the lock and destroy the lock safely after
                // because we know for sure that no other thread would try to lock it in between)
                pthread_mutex_unlock(&(to_delete->mutex));
                delete to_delete; // mutex destroyed in Node's d'tor

                return true; // success
            }

            Node* prev = iter;
            iter=iter->next;
            pthread_mutex_unlock(&(prev->mutex));
        }

        pthread_mutex_unlock(&(iter->mutex)); // unlock
        return false;
    }

    /**
     * Returns the current size of the list
     * @return current size of the list
     */
    unsigned int getSize() {
        int retVal = 0;
        pthread_mutex_lock(&size_mutex);
        retVal = size;
        pthread_mutex_unlock(&size_mutex);
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

    // for testing
    bool isSorted(){
        pthread_mutex_lock(&(head->mutex));
        if(!head->next) {
            pthread_mutex_unlock(&(head->mutex));
            return true;
        }else{
            pthread_mutex_lock(&head->next->mutex);
            pthread_mutex_unlock(&(head->mutex));
        }
        Node* prev = head->next;
        Node* curr = head->next->next;
        while(curr) {
            pthread_mutex_lock(&curr->mutex);
            if(prev->data >= curr->data) {
                pthread_mutex_unlock(&curr->mutex);
                pthread_mutex_unlock(&prev->mutex);
                return false;
            }
            pthread_mutex_unlock(&prev->mutex);
            prev = curr;
            curr = curr->next;
        }
        pthread_mutex_unlock(&prev->mutex);
        return true;
    }

};

#endif //THREAD_SAFE_LIST_H_
