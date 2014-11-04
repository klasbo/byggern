#include "fifoqueue.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

typedef struct fifonode_t fifonode_t;
struct fifonode_t {
    uint8_t         type;
    void*           data;
    
    uint16_t        size;
    fifonode_t*    next;
};

struct fifoqueue_t {
    fifonode_t*    front;
};



fifoqueue_t* new_fifoqueue(void){
    fifoqueue_t* q = malloc(sizeof(fifoqueue_t));
    q->front = NULL;
    return q;
}



void enqueue(fifoqueue_t* q, uint8_t type, void* data, uint16_t size){
    fifonode_t* newnode = malloc(sizeof(fifonode_t));
    newnode->type = type;
    newnode->data = malloc(size);
    newnode->size = size;
    newnode->next = NULL;
    
    memcpy(newnode->data, data, size);

    if(q->front == NULL){
        q->front = newnode;
        } else {
        fifonode_t* n = q->front;
        while(n->next != NULL){
            n = n->next;
        }
        n->next = newnode;
    }
}



void dequeue(fifoqueue_t* q, void* recv){
    memcpy(recv, q->front->data, q->front->size);

    fifonode_t* del = q->front;
    
    q->front = q->front->next;
    
    free(del->data);
    free(del);
}



uint8_t front_type(fifoqueue_t* q){
    if(q->front == NULL){
        return 0;
        } else {
        return q->front->type;
    }
}



void print_fifonode_t(fifonode_t* n){
    printf_P(PSTR("fifonode_t(%d, %p, %u, %p)\n"), n->type, n->data, n->size, n->next);
}



void print_fifoqueue_t(fifoqueue_t* q){
    for(fifonode_t* n = q->front; n != NULL; n = n->next){
        print_fifonode_t(n);
    }
}



void free_fifoqueue(fifoqueue_t** q){
    
    while(front_type(*q) != 0){
        fifonode_t* del = (*q)->front;
        printf("%d\n", del->type);
        (*q)->front = (*q)->front->next;
        
        free(del->data);
        free(del);
    }
    
    free(*q);
}