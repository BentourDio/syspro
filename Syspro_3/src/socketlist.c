/*
    Dionysios Mpentour 1115201300115

    This is an ordinary linked list, which each node has socket related data.
    Specifically file descriptor related to the socket.
    A fd assigned to a socket can be used for both reading and writing, but I keep a node 
    for a reading and one for writing as per prj2 implementation. The fd of both reading and
    writing nodes point to the same fd created from the socket(...).This is done in order to keep
    the usability of the program without changing too much stuff.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>

#include "socketlist.h"

struct Record_list *Create_Record(struct Record_list *list_ptr) {

    struct Record_list *temp;

    temp = (struct Record_list *) malloc(sizeof (struct Record_list));
    
    temp->fd = 0;

    if (list_ptr == NULL) { // einai to prwto RecordID poy ginetai eggrafi
        temp->next = NULL;
        list_ptr = temp;
    } else { // eisagwgi stin arxi tis listas
        temp->next = list_ptr;
        list_ptr = temp;
    }

    return list_ptr;
}

void Destroy_Record_list(struct Record_list *list_ptr) {
    struct Record_list *temp;
    struct Record_list *next;
    temp = list_ptr;
    while (temp != NULL) {
        next = temp->next;
        free(temp);
        temp = next;
    }
    list_ptr = NULL;
}

void Print_Record_List(struct Record_list *list_ptr) {
    struct Record_list *temp;
    temp = list_ptr;

    if (temp == NULL) {
        printf("RecordID is empty.\n");
    } else {
        while (temp != NULL) {
            printf("%d\n", temp->fd);
            temp = temp->next;
        }
    }
}

void Destroy_Sockets_From_List(struct Record_list *list_ptr) {
    struct Record_list *temp;
    temp = list_ptr;

    if (temp == NULL) {
        return;
    } else {
        while (temp != NULL) {
            if (temp->fd != 0) {
                shutdown(temp->fd, SHUT_RDWR);
                close(temp->fd);    
            }

            temp = temp->next;
        }
    }
}