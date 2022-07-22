/*
    Dionysios Mpentour 1115201300115

    In this file the functions for the countrydir structure are implemented.
    The country directory structure is used to store the directories 
    of the countries in the coordinator. The structure also has pointers
    to other types of structures. Specifically the Record List where the names
    of the pipes which the child/monitor uses to transfer data related to the
    country. Also the pid of the child/monitor is stored and a pointer to a
    stats structure, which is used to implement the travelStats function
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "countrydir_list.h"
#include "countrylist.h"
#include "pipelist.h"
#include "travelReq_list.h"

struct CountryDirList *CountryDirList_Init(char *country) {

    struct CountryDirList *newNode =
            (struct CountryDirList *) malloc(sizeof (struct CountryDirList));

    newNode->country = (char *) malloc(sizeof (char) * strlen(country) + 1);
    strcpy(newNode->country, country);

    newNode->next = NULL;
    newNode->previous = NULL;
    newNode->output = NULL;
    newNode->input = NULL;
    newNode->pid = 0;
    newNode->stats_head = NULL;

    StatsHead_Init(&newNode->stats_head);

    return newNode;
}
// inserting in alphabetical order

void CountryDirList_Insert(struct CountryDirList **head, char *country) {

    struct CountryDirList *newNode = CountryDirList_Init(country);
    char cur, new;
    if (*head != NULL) {
        cur = (*head)->country[0];
        new = country[0];
    }

    if (*head == NULL) {
        // list is empty
        *head = newNode;
        //printf(" 1 country to %s\n", country);
    } else if (*head != NULL && new <= cur) {

        //printf(" 2 country to %s\n", country);
        newNode->next = *head;
        (*head)->previous = newNode;
        *head = newNode;
    } else {
        struct CountryDirList *temp = *head;
        cur = temp->country[0];
        //printf("[1] the letter is %c and country: %s\n", cur, country);

        while (temp->next != NULL && new > cur) {

            temp = temp->next;
            cur = temp->country[0];
            //printf("[2] the letter is %c %s and country: %s\n", cur, temp->country,
            //       country);
        }
        if (cur >= new) {
            newNode->next = temp->previous->next;
            newNode->previous = temp->previous;
            temp->previous->next = newNode;
            temp->previous = newNode;
        }
        if (new >= cur) {
            // that means that temp->next == NULL
            if (temp->next == NULL) {
                temp->next = newNode;
                newNode->previous = temp;
            }
        }
    }
}

struct CountryDirList *CountryDirList_Search(struct CountryDirList *head,
        char *country) {
    struct CountryDirList *temp = head;

    while (temp != NULL) {
        if (strcmp(temp->country, country) == 0) {
            return temp;
        }
        temp = temp->next;
    }

    return NULL;
}

void CountryDirList_Purge(struct CountryDirList *head) {
    struct CountryDirList *temp, *aux;
    temp = head;

    while (temp != NULL) {
        aux = temp;
        temp = temp->next;

        //free(aux->input);
        //free(aux->output);
        StatsHead_Purge(aux->stats_head);
        free(aux->country);
        free(aux);
    }
}

void CountryDirList_Print(struct CountryDirList *head) {
    struct CountryDirList *temp = head;

    while (temp != NULL) {
        if (temp->input && temp->output) {
            printf("%20s %20s %5d %20s %5d %3d \n", temp->country, temp->input->pipeName, temp->input->fd, temp->output->pipeName, temp->output->fd, temp->pid);
        } else {
            printf("%20s %20s %5d %20s %5d %3d \n", temp->country, "?", 0, "?", 0, 0);
        }
        temp = temp->next;
    }
    printf("\n");
}