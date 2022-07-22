/*
  Dionysios Mpentour 1115201300115

  Skip list data structure. This works fine. I got a good grade on PRJ-1
*/

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

struct Skip_List_Node *Skip_List_Node_Init(int citizenID, int day, int month,
                                           int year, int vstatus) {
  struct Skip_List_Node *newNode;

  newNode = (struct Skip_List_Node *)malloc(sizeof(struct Skip_List_Node));

  DateRecord_Init(&(newNode->date_ptr), day, month, year, vstatus);

  newNode->citizenID = citizenID;

  newNode->maxSize = MAXLVL; // only for header.

  newNode->info_ptr = NULL;

  newNode->level = (struct Skip_List_Node **)malloc(
      sizeof(struct Skip_List_Node *) * (newNode->maxSize));

  for (int i = 0; i < newNode->maxSize; i++) {
    newNode->level[i] = NULL;
  }

  return newNode;
}

struct Skip_List_Node *Skip_List_Insert(struct Skip_List_Node *head,
                                        int citizenID, int day, int month,
                                        int year, int vstatus,
                                        struct Hash_Entry *hptr) {
  struct Skip_List_Node *temp = head;

  struct Skip_List_Node **update;

  int rlevel = levelCasino(MAXLVL);

  if (head->level[0] == NULL) { // first element to be inserted

    struct Skip_List_Node *newNode =
        Skip_List_Node_Init(citizenID, day, month, year, vstatus);

    newNode->info_ptr = hptr;

    for (int i = 0; i < rlevel; i++) {
      head->level[i] = newNode;
    }

    /*if (newNode->info_ptr != NULL) {
      printf("[Inserted Skip List node with key: %d and date %d|%d|%d "
             "name:%s|surname:%s|country:%s|age: %d\n ",
             newNode->citizenID, newNode->day, newNode->month, newNode->year,
             newNode->info_ptr->name, newNode->info_ptr->surname,
             newNode->info_ptr->country, newNode->info_ptr->age);
    }*/
    return head;
  }

  update = (struct Skip_List_Node **)malloc(sizeof(struct Skip_List_Node *) *
                                            (MAXLVL));
  // array in order to keep the position of insertion
  // maxSize +1 in case rand gives 0 so that we at list have level[0]
  for (int i = 0; i < MAXLVL; i++) { // initialize array to NULL
    update[i] = NULL;
  }

  // printf("1\n");
  // printf("the current level is %d\n",head->clevel);
  for (int i = MAXLVL - 1; i >= 0; i--) {
    // printf("2\n");
    while (temp->level[i] != NULL && temp->level[i]->citizenID < citizenID) {
      temp = temp->level[i];
    }
    update[i] = temp;
  }

  temp = temp->level[0];

  // printf("random level is %d\n",rlvl);
  if (temp == NULL || temp->citizenID != citizenID) {
    struct Skip_List_Node *newNode =
        Skip_List_Node_Init(citizenID, day, month, year, vstatus);

    newNode->info_ptr = hptr;

    for (int i = 0; i < rlevel; i++) {
      newNode->level[i] = update[i]->level[i];
      update[i]->level[i] = newNode;
    }

    /*if (newNode->info_ptr != NULL) {
      printf("[Inserted Skip List node with key: %d and date %d|%d|%d "
             "name:%s|surname:%s|country:%s|age:%d\n",
             newNode->citizenID, newNode->day, newNode->month, newNode->year,
             newNode->info_ptr->name, newNode->info_ptr->surname,
             newNode->info_ptr->country, newNode->info_ptr->age);
    }*/
  }

  free(update);
  // printf("please dont fkin seg on me. Insert success with
  // citizenID:%d\n",citizenID);
  return head;
}

void Print_Skip_List(struct Skip_List_Node *head) {

  if (head == NULL) {
    printf("skip list is empty\n");
    return;
  }

  for (int i = MAXLVL - 1; i >= 0; i--) {
    struct Skip_List_Node *temp = head->level[i];

    printf("Level :%d ", i);

    while (temp != NULL) {
      // printf("Level :%d ",i);
      printf(" %d ", temp->citizenID);
      temp = temp->level[i];
    }
    printf("\n");
  }
}

void Test_Sequence_Skip_List(struct Skip_List_Node *head) {

  if (head == NULL) {
    return;
  }

  for (int i = MAXLVL - 1; i >= 0; i--) {
    struct Skip_List_Node *temp = head->level[i];

    while (temp != NULL) {
      // printf("Level :%d ",i);
      int x = temp->citizenID;

      if (temp->level[i]) {
        int y = temp->level[i]->citizenID;

        if (y < x) {
          exit(7);
        }
      }

      temp = temp->level[i];
    }
  }
}

struct Skip_List_Node *Skip_List_Search(struct Skip_List_Node *head,
                                        int citizenID) {

  struct Skip_List_Node *temp = head;

  for (int i = MAXLVL - 1; i >= 0; i--) {
    while (temp->level[i] != NULL && temp->level[i]->citizenID < citizenID) {
      temp = temp->level[i];
    }
  } // end for

  temp = temp->level[0];

  if (temp != NULL && temp->citizenID == citizenID) {
    return temp;
  } else {
    return NULL;
  }
}

void Skip_List_Delete(struct Skip_List_Node **head, int citizenID) {
  struct Skip_List_Node *temp = Skip_List_Search(*head, citizenID);
  if (temp == NULL) {
    printf("[CitizenID does not exist in current skip list\n");
    return;
  }
  temp = (*head);
  struct Skip_List_Node **update;

  update = (struct Skip_List_Node **)malloc(sizeof(struct Skip_List_Node *) *
                                            (MAXLVL));
  // array in order to keep the position of insertion

  for (int i = 0; i < MAXLVL; i++) { // initialize array to NULL
    update[i] = NULL;
  }

  for (int i = MAXLVL - 1; i >= 0; i--) {
    while (temp->level[i] != NULL && temp->level[i]->citizenID < citizenID) {
      temp = temp->level[i];
    }
    update[i] = temp;
  }

  temp = temp->level[0];

  if (temp != NULL && temp->citizenID == citizenID) {
    for (int i = 0; i < MAXLVL; i++) {
      if (update[i]->level[i] != temp) {
        break;
      }
      update[i]->level[i] = temp->level[i];
    }
  }

  free(update);
  free(temp->level);
  free(temp->date_ptr);
  free(temp);
}

void Skip_List_Purge(struct Skip_List_Node *head) {

  struct Skip_List_Node *temp = head->level[0];
  struct Skip_List_Node *throwaway;
  while (temp != NULL) {
    throwaway = temp;
    temp = temp->level[0];

    free(throwaway->level);
    DateRecord_Delete(throwaway->date_ptr);
    free(throwaway);
  }

  free(head->level);
  free(head->date_ptr);
  free(head);
}

void Skip_List_Print_Level(struct Skip_List_Node *head) {
  struct Skip_List_Node *temp = head->level[0];

  while (temp != NULL) {
    printf("%d %s %s %s %d\n", temp->citizenID, temp->info_ptr->name,
           temp->info_ptr->surname, temp->info_ptr->country,
           temp->info_ptr->age);
    temp = temp->level[0];
  }
}