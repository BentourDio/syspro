/*
  Dionysios Mpentour 11152013001115
  
  
  functions of the Countrylist structure are stored here. This structure in mainly used
  for the PRJ-1 for the populationStatus and popStatusByAge functions. In PRJ-2
  the structure stores the countries whose directories the individual monitor/child contains
  data about. Also it has a same Countrylist pointer where the data
  about the subdirectory's files of each country are storedl, in order not to reopen
  already opened files in the addVaccinationRecords function
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "countrylist.h"
#include "functions.h"

void CountryList_Init(struct CountryList **newCNode, char *path) {
  (*newCNode) = (struct CountryList *)malloc(sizeof(struct CountryList));

  (*newCNode)->path = (char *)malloc(sizeof(char) * strlen(path) + 1);
  strcpy((*newCNode)->path, path);
  
  char * t = strrchr(path, '/') + 1;
  
  (*newCNode)->country = malloc(sizeof(char)*(strlen(t) + 1));
  strcpy((*newCNode)->country, t);
  
  (*newCNode)->filename = NULL;

  (*newCNode)->next = NULL;
  (*newCNode)->filelist = NULL;
}

struct CountryList *CountryList_Insert(struct CountryList **head, char *path) {
  struct CountryList *newCNode;

  CountryList_Init(&newCNode, path);

  newCNode->next = (*head);

  (*head) = newCNode; // insertion at the start of simply linked list

  return newCNode;
}


struct CountryList* CountryList_InsertFile(struct CountryList ** head, char* filename){
  struct CountryList * newNode;
  
  newNode =  (struct CountryList *) malloc (sizeof(struct CountryList));
  newNode->filename = (char* ) malloc (sizeof(char)*(strlen (filename)+1));
  strcpy(newNode->filename,filename);

  newNode->country =NULL;
  newNode->path=NULL;
  newNode->filelist =NULL;
  newNode->next = NULL;

  if ( (*head) == NULL)
    (*head) = newNode;
  else{
    newNode->next = (*head);
    (*head) = newNode;
  }

  return newNode;
}

struct CountryList* CountryList_SearchFile(struct CountryList* head, char* key){
  struct CountryList* countryfile = head;

  while(countryfile!=NULL && strcmp(countryfile->filename,key) != 0)
    countryfile = countryfile->next;

  if(countryfile!=NULL && strcmp(countryfile->filename,key) == 0)
    return countryfile;
  else
   return NULL;
}


struct CountryList* CountryList_SearchPath(struct CountryList* head, char* key){
  struct CountryList* path = head;

  while(path!=NULL && strcmp(path->path,key) != 0)
    path = path->next;

  if(path!=NULL && strcmp(path->path,key) == 0)
    return path;
  else
   return NULL;
}

void CountryList_Purge(struct CountryList *head) {
  struct CountryList *temp, *aux;

  temp = head;

  while (temp != NULL) {
    aux = temp;
    temp = temp->next;
    CountryList_Purge(aux->filelist);
    if(aux->country !=NULL )
      free(aux->country);
    if(aux->path != NULL)
      free(aux->path);
    if(aux->filename !=NULL)
      free(aux->filename);
    free(aux);
  }
}

void CountryList_Print(struct CountryList *head) {
  struct CountryList *temp = head;

  while (temp != NULL) {
    printf("%s\n",temp->country);
    temp = temp->next;
  }
}

void CountryList_PrintFiles(struct CountryList* head){
  struct CountryList *temp = head;

  while (temp != NULL) {
    printf("%s\n",temp->filename);
    temp = temp->next;
  }
}

struct CountryList *CountryList_Search(struct CountryList *head,
                                       char *country) {
  struct CountryList *temp = head;

  while (temp != NULL && strcmp(temp->country, country) != 0) {
    temp = temp->next;
  }

  if (temp != NULL && strcmp(temp->country, country) == 0) {
    return temp;
  } else {
    return NULL;
  }
}

void PrintPopStatusAgeVirusDates(struct CountryList *head) {
  
}