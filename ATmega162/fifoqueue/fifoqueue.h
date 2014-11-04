#pragma once

#include <stdint.h>



/** An untyped first-in first-out queue.
    Recommended usage:
        fifoqueue_scoped* q = new_fifoqueue();
        A a; // instance of type A, with type id "typeA"

        switch(front_type(q)){
        case typeA:
            dequeue(q, &a);
            // handle type A here
        default:
            // includes case 0 (queue empty)
        }

    call enqueue() in interrupts or when handling other types
*/



/** The base type. Prefer declaring queues with fifoqueue_scoped.
*   Initialize a new queue with new_fifoqueue();
*/
typedef struct fifoqueue_t fifoqueue_t;


/// fifoqueue_scoped is a wrapper around an fifoqueue_t, such that it is automatically freed at scope exit.
#define fifoqueue_scoped __attribute__((cleanup(free_fifoqueue))) fifoqueue_t


/// Returns a fresh fifoqueue_t from the heap.
fifoqueue_t* new_fifoqueue(void);


/** Copies data of _size size into the back of the queue.
*   The caller defines a type id, that can be retreived with frontType()
*       Type id 0 is reserved (see frontType())
*   enqueue does not do a deep copy of data
*/
void enqueue(fifoqueue_t* q, uint8_t type, void* data, uint16_t size);


/** Copies the front element of the queue into recv
*   Use frontType() to get the type of the front element
*/
void dequeue(fifoqueue_t* q, void* recv);

/** Returns the type id of the front element, as set in enqueue()
    Returns 0 if the queue is empty
*/
uint8_t front_type(fifoqueue_t* q);


void print_fifoqueue_t(fifoqueue_t* q);


/** Frees all data in an fifoqueue_t
*   Called by fifoqueue_scoped on scope exit
*/
void free_fifoqueue(fifoqueue_t** q);
