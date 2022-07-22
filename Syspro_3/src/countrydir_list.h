#ifndef COUNTRYDIR_LIST_H
#define COUNTRYDIR_LIST_H


#include <signal.h>

typedef struct Stats_Head* stats;// forward declaration


struct CountryDirList{
    char* country;
    
    struct Record_list * input;
    struct Record_list * output;
    pid_t pid;
    int monitor_id;
    
    struct CountryDirList* next;
    struct CountryDirList *previous;
    struct Stats_Head *stats_head; 
};

struct CountryDirList* CountryDirList_Init(char* country, int id);

void CountryDirList_Insert(struct CountryDirList** head, char* country, int id);

struct CountryDirList *CountryDirList_Search(struct CountryDirList *head,
                                             char *country);

void CountryDirList_Purge(struct CountryDirList *head);

void CountryDirList_Print(struct CountryDirList *head);
#endif