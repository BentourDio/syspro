/*
    Dionysios Mpentour 1115201300115
    Cyclic buffer aka fifo queue is created here. The queue parameter is
    defined as an extern global in the ConsumersQueue.h header file.
    Appropriate mutexes and signals has been implemented.

*/


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <malloc.h>

#include "ConsumersQueue.h"

void ConsumerQueue_init(int capacity) {
    queue = (struct ConsumerQueue *) malloc(sizeof (struct ConsumerQueue));
    queue->start = 0;
    queue->end = -1;
    queue->count = 0;
    queue->capacity = capacity;

    queue->data = (char**) malloc(capacity * sizeof (char*));

    pthread_mutex_init(&queue->mtx, 0);
    pthread_cond_init(&queue->cond_nonempty, 0);
    pthread_cond_init(&queue->cond_nonfull, 0);
    
    queue->cancel = false;
}

void ConsumerQueue_Purge() {
    pthread_cond_destroy(&(queue)->cond_nonempty);
    pthread_cond_destroy(&(queue)->cond_nonfull);
    pthread_mutex_destroy(&(queue)->mtx);

    free(queue->data);

    free(queue);

    queue = NULL;
}

void ConsumerQueue_Place(char * item) {
    pthread_mutex_lock(&(queue)->mtx);
    while ((queue)->count >= (queue)->capacity) {
        pthread_cond_wait(&(queue)->cond_nonfull, &(queue)->mtx);
    }
    (queue)->end = ((queue)->end + 1) % (queue)->capacity;
    (queue)->data[(queue)->end] = item;
    (queue)->count++;
    pthread_cond_signal(&(queue)->cond_nonempty);
    pthread_mutex_unlock(&(queue)->mtx);
}

char *ConsumerQueue_Obtain() {
    char * data = NULL;
    
    pthread_mutex_lock(&(queue)->mtx);
    while ((queue)->count <= 0 && (queue)->cancel == false) {
        pthread_cond_wait(&(queue)->cond_nonempty, &(queue)->mtx);
    }
    if ((queue)->cancel == true) {
        pthread_mutex_unlock(&(queue)->mtx);
        return NULL;
    }
    data = (queue)->data[(queue)->start];
    (queue)->start = ((queue)->start + 1) % (queue)->capacity;
    (queue)->count--;
    pthread_cond_signal(&(queue)->cond_nonfull);
    pthread_mutex_unlock(&(queue)->mtx);
    return data;
}