
#ifndef CONSUMERSQUEUE_H
#define CONSUMERSQUEUE_H

#include <stdbool.h>


typedef struct ConsumerQueue* q;

struct ConsumerQueue {
    char ** data;
    int start;
    int end;
    int count;
    int capacity;

    pthread_mutex_t mtx;
    pthread_cond_t cond_nonempty;
    pthread_cond_t cond_nonfull;
    bool cancel;
};

extern struct ConsumerQueue* queue;


void ConsumerQueue_init(int capacity);

void ConsumerQueue_Purge();

void ConsumerQueue_Place( char * item);

char *ConsumerQueue_Obtain();

#endif /* CONSUMERSQUEUE_H */

