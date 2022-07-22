#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "app.h"
#include "date_record.h"
#include "functions.h"
#include "myhashtable.h"
#include "mylist.h"
#include "myskiplist.h"
// Disease_List functions

struct Disease_List *Disease_List_Init(char *virusName) {
  struct Disease_List *newNode;

  newNode = (struct Disease_List *)malloc(sizeof(struct Disease_List));

  newNode->virusName = (char *)malloc(sizeof(char) * (strlen(virusName) + 1));
  strcpy(newNode->virusName, virusName);

  newNode->vacc_head = NULL;
  newNode->unvacc_head = NULL;

  newNode->vaccinated_sum = 0;
  newNode->not_vaccinated_sum = 0;

  return newNode;
}

void Disease_List_Insert(struct Disease_List **head, char *virusName,
                         struct Hash_Entry *hptr) {

  struct Disease_List *temp;

  temp = Disease_List_Init(virusName); // initialize node

  temp->next = (*head);

  (*head) = temp; // insertion at the start of the list
  // initialize skip lists for disease
  temp->unvacc_head = Skip_List_Node_Init(-1, MAXLVL, -1, -1, -1);

  temp->vacc_head = Skip_List_Node_Init(-1, MAXLVL, -1, -1, -1);
}

struct Disease_List *Disease_List_Search(struct Disease_List *head,
                                         char *virusName) {
  struct Disease_List *temp = head;

  if (head == NULL) // first element of the linked list
    return NULL;

  while (temp != NULL && strcmp(temp->virusName, virusName) != 0) {
    temp = temp->next;
  }

  if (temp != NULL && (strcmp(temp->virusName, virusName)) == 0) {
    return temp;
  } else {
    return NULL;
  }
}

void Disease_List_Purge(struct Disease_List *head) {
  struct Disease_List *temp, *thr;

  temp = head;

  while (temp != NULL) {
    thr = temp;

    temp = temp->next;

    Skip_List_Purge(thr->vacc_head);
    Skip_List_Purge(thr->unvacc_head);

    free(thr->virusName);
    free(thr);
  }
}

void Disease_List_Print(struct Disease_List *head) {

  struct Disease_List *temp = head;

  while (temp != NULL) {
    printf("Virus name: %s\n", temp->virusName);

    if (temp->vacc_head != NULL) {
      printf("########    Vacc List:\n");
      Print_Skip_List(temp->vacc_head);
    }

    if (temp->unvacc_head != NULL) {
      printf("#######     Non Vacc list:\n");
      Print_Skip_List(temp->unvacc_head);
    }
    temp = temp->next;
  }
}
