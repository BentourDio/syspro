/*
    Dionysios Mpentour 1115201300115

    TravelRequestList structure is used to store the requests from the
    travelRequest function and in order to answer the TravelStats query.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#include "travelReq_list.h"

void StatsHead_Init(struct Stats_Head **stats_head){
    (*stats_head) = (struct Stats_Head *) malloc (sizeof(struct Stats_Head));
    (*stats_head)->accepted_list = NULL;
    (*stats_head)->rejected_list = NULL;

    (*stats_head)->rejectedNum = 0;
    (*stats_head)->acceptedNum = 0;
}

struct TravelReqList* TravelReqList_Init(char* citizenID,char* date, char* countryTo){
    struct TravelReqList* newNode = (struct TravelReqList*) malloc (sizeof(struct TravelReqList));

    newNode->citizenID = (char*) malloc (sizeof(char)*strlen(citizenID)+1);
    strcpy(newNode->citizenID,citizenID);
    newNode->countryTo = (char *)malloc(sizeof(char) * strlen(countryTo) + 1);
    strcpy(newNode->countryTo, countryTo);
    
    char* temp = (char*) malloc (sizeof(char)*strlen(date)+1);
    strcpy(temp,date);

    int day,month,year;
    day   = atoi(strtok(temp,"-"));
    month = atoi(strtok(NULL, "-"));
    year  = atoi(strtok(NULL, ""));

    newNode->day  = day;
    newNode->month= month;
    newNode->year = year;

    newNode->next = NULL;


    free(temp);
    return newNode;
}

void TravelReqList_Insert(struct TravelReqList** head, char* citizenID, char* date, char* countryTo){
    struct TravelReqList* newNode = TravelReqList_Init(citizenID,date,countryTo);
    //insert at the start of the list
    if ((*head) == NULL){
        (*head) = newNode;
    }else{
        newNode->next = (*head);
        (*head) = newNode;
    }
}

void StatsHead_Purge(struct Stats_Head* stats_head){
    TravelReqList_Purge(stats_head->accepted_list);
    TravelReqList_Purge(stats_head->rejected_list);
    free(stats_head);
}

void TravelReqList_Purge(struct TravelReqList *head){
    struct TravelReqList *aux, *temp = head;

    while(temp!=NULL){
        aux = temp;
        temp = temp->next;
        free(aux->citizenID);
        free(aux->countryTo);
        free(aux);
    }
}

void TravelReqList_Print(struct TravelReqList *head){
    struct TravelReqList* temp = head;

    while(temp != NULL){
        printf("CitizenID:%s CountryTo:%s Date:%d-%d-%d\n",temp->citizenID,temp->countryTo,temp->day,temp->month,temp->year);
        temp = temp->next;
    }
}