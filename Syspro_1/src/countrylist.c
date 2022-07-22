#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "countrylist.h"
#include "functions.h"

void CountryList_Init(struct CountryList **newCNode, char *country) {
  (*newCNode) = (struct CountryList *)malloc(sizeof(struct CountryList));

  (*newCNode)->country = (char *)malloc(sizeof(char) * strlen(country) + 1);
  strcpy((*newCNode)->country, country);

  (*newCNode)->vacc_summ = 0;
  (*newCNode)->vacc_summ_overall = 0;

  (*newCNode)->unvacc_summ = 0;

  (*newCNode)->agegroup1 = 0; // 0-20
  (*newCNode)->agegroup2 = 0; // 20-40
  (*newCNode)->agegroup3 = 0; // 40-60
  (*newCNode)->agegroup4 = 0; // 60+

  (*newCNode)->agegroup1_unvacc = 0;
  (*newCNode)->agegroup2_unvacc = 0;
  (*newCNode)->agegroup3_unvacc = 0;
  (*newCNode)->agegroup4_unvacc = 0;

  (*newCNode)->overall1 = 0;
  (*newCNode)->overall2 = 0;
  (*newCNode)->overall3 = 0;
  (*newCNode)->overall4 = 0;

  (*newCNode)->next = NULL;
}

struct CountryList *CountryList_Insert(struct CountryList **head,
                                       char *country) {
  struct CountryList *newCNode;

  CountryList_Init(&newCNode, country);

  // groupIncrement(&newCNode, age); // increments the age group accordingly

  newCNode->next = (*head);

  (*head) = newCNode; // insertion at the start of simply linked list

  return newCNode;
}

void groupIncrement(struct CountryList **node, int age) {

  int agegroup = ageGroup(age);

  if (agegroup == 1) { // 0-20
    (*node)->agegroup1++;
  }
  if (agegroup == 2) { // 20-40
    (*node)->agegroup2++;
  }
  if (agegroup == 3) { // 40-60
    (*node)->agegroup3++;
  }
  if (agegroup == 4) { // 60+
    (*node)->agegroup4++;
  }
}

void nonvaccIncrement(struct CountryList **node, int age) {
  int agegroup = ageGroup(age);

  if (agegroup == 1) { // 0-20
    (*node)->agegroup1_unvacc++;
  }
  if (agegroup == 2) { // 20-40
    (*node)->agegroup2_unvacc++;
  }
  if (agegroup == 3) { // 40-60
    (*node)->agegroup3_unvacc++;
  }
  if (agegroup == 4) { // 60+
    (*node)->agegroup4_unvacc++;
  }
}

void overallIncrement(struct CountryList **node, int age) {
  int agegroup = ageGroup(age);

  if (agegroup == 1) { // 0-20
    (*node)->overall1++;
  }
  if (agegroup == 2) { // 20-40
    (*node)->overall2++;
  }
  if (agegroup == 3) { // 40-60
    (*node)->overall3++;
  }
  if (agegroup == 4) { // 60+
    (*node)->overall4++;
  }
}

void CountryList_Purge(struct CountryList *head) {
  struct CountryList *temp, *aux;

  temp = head;

  while (temp != NULL) {
    aux = temp;
    temp = temp->next;
    free(aux->country);
    free(aux);
  }
}

void CountryList_Print(struct CountryList *head) {
  struct CountryList *temp = head;

  // printf("\t\t\t[Country list print]\n");

  while (temp != NULL) {
    printf("Country:%s\n0-20:%d\n20-40:%d \n40-60:%d \n60+  :%d \n\n",
           temp->country, temp->agegroup1, temp->agegroup2, temp->agegroup3,
           temp->agegroup4);

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