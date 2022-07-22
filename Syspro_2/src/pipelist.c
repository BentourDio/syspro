/*
    Dionysios Mpentour 1115201300115

    This is an ordinary linked list, which each node has pipe related data.
    Specifically the pipname and the file descriptor related to the named
    pipe. The first pipe is for writing and the second for reading. So even
    numbers are for writing and odd numbers for reading. (we start from 0).
    So the coordinator uses 2 nodes of this list for the back and forth communication
    with each of the monitors processes.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "pipelist.h"

struct Record_list *Create_Record(struct Record_list *list_ptr, char *pipename) {

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

    temp->pipeName = (char *) malloc(sizeof (char) * (strlen(pipename) + 1));
    strcpy(temp->pipeName, pipename);

    return list_ptr;
}

void Destroy_Record_list(struct Record_list *list_ptr) {
    struct Record_list *temp;
    struct Record_list *next;
    temp = list_ptr;
    while (temp != NULL) {
        next = temp->next;
        free(temp->pipeName);
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
            printf("%s\n", temp->pipeName);
            temp = temp->next;
        }
    }
}

void Create_Pipes_From_List(struct Record_list *list_ptr) {
    struct Record_list *temp;
    temp = list_ptr;

    if (temp == NULL) {
        return;
    } else {
        while (temp != NULL) {
            unlink(temp->pipeName);

            int error = mkfifo(temp->pipeName, 0666);

            if (error == -1) {
                printf("Error while creating %s \n", temp->pipeName);
                exit(1);
            }
            temp = temp->next;
        }
    }
}

void Destroy_Pipes_From_List(struct Record_list *list_ptr) {
    struct Record_list *temp;
    temp = list_ptr;

    if (temp == NULL) {
        return;
    } else {
        while (temp != NULL) {
            unlink(temp->pipeName);

            temp = temp->next;
        }
    }
}