/*
    Dionysios Mpentour 1115201300115

    Hash Table and respective functions. The Hash Table is used by the monitor/child
    proccesses to store each individual unique citizenID and respective data. the data
    is stored only in the hashtable to avoid duplication and be accessed in O(1) fashion.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "myhashtable.h"

struct Hashtable Hashtable_Creation(int size) {

    struct Hashtable h;
    int i;

    h.htable = (struct Hash_Entry **) malloc(sizeof (struct Hash_Entry *) * size);

    for (i = 0; i < size; i++) {
        h.htable[i] = NULL;
    }

    h.size = size;

    return (h);
}

void Hash_Entry_Init(struct Hash_Entry **entry, char *citizenID, char *name,
        char *surname, char *country, char *virusName, int age) {

    struct Hash_Entry *temp = (*entry);

    temp->name = (char *) malloc(sizeof (char) * (strlen(name) + 1));
    strcpy(temp->name, name);

    temp->surname = (char *) malloc(sizeof (char) * (strlen(surname) + 1));
    strcpy(temp->surname, surname);

    temp->country = (char *) malloc(sizeof (char) * (strlen(country) + 1));
    strcpy(temp->country, country);

    temp->virusName = (char *) malloc(sizeof (char) * (strlen(virusName) + 1));
    strcpy(temp->virusName, virusName);

    temp->citizenID = (char *) malloc(sizeof (char) * (strlen(citizenID) + 1));
    strcpy(temp->citizenID, citizenID);

    temp->age = age;


    temp->next = NULL;
}

int Htable_Insert(struct Hashtable h, char *citizenID, char *name, char *surname, char *country, char *virusName, int age) {
    int pos;
    struct Hash_Entry *entry, *temp;

    pos = Hash_key(citizenID, h.size);

    if (h.htable[pos] == NULL) { // first element of this position.
        entry = (struct Hash_Entry *) malloc(sizeof (struct Hash_Entry));
        if (entry != NULL) {
            Hash_Entry_Init(&entry, citizenID, name, surname, country, virusName, age);

        } else {
            return -1;
        }
        h.htable[pos] = entry; // insertion at the start of the hashlist
        return 0;
    }

    // insertion in existing hash list
    temp = h.htable[pos]; // pointing to the first item of simply linked list
    entry = (struct Hash_Entry *) malloc(sizeof (struct Hash_Entry));

    if (entry != NULL) {
        Hash_Entry_Init(&entry, citizenID, name, surname, country, virusName, age);
        entry->next = temp;
    } else {
        return -1;
    }

    h.htable[pos] = entry;

    return 0;
}

/*hash function with returns a key for the hash array
  djb2
http://www.cse.yorku.ca/~oz/hash.html
 */
int Hash_key(char *name, int size) {
    unsigned long hash = 5381;
    int c = 0;

    while (1) {
        c = *name;
        name++;
        if (c == '\0') {
            break;
        }
        hash = ((hash << 5) + hash) + c;
    }

    hash = hash % size;

    return (hash);
}

void Hashtable_Print(struct Hashtable h) {
    for (int i = 0; i < h.size; i++) {
        Hash_Lists_Print(h.htable[i], i);
    }
}

void Hash_Lists_Print(struct Hash_Entry *p, int pos) {
    struct Hash_Entry *temp = p;

    if (temp == NULL) {
        return;
    }
    
    printf("hash pos: %d: \n", pos);

    while (temp != NULL) {

        printf("\t\t %s | %s | %s | %s | %s | %d \n ",
                temp->citizenID, temp->name, temp->surname, temp->country,
                temp->virusName, temp->age);
        temp = temp->next;
    }
}

void Hashtable_Purge(struct Hashtable h) {
    struct Hash_Entry *temp;

    for (int i = 0; i < h.size; i++) {
        temp = h.htable[i];

        Hash_List_Purge(temp);
    }
    free(h.htable);
}

void Hash_List_Purge(struct Hash_Entry *p) {
    struct Hash_Entry *temp, *aux;

    temp = p;

    while (temp != NULL) {
        aux = temp;
        temp = temp->next;
        free(aux->citizenID);
        free(aux->name);
        free(aux->surname);
        free(aux->virusName);
        free(aux->country);
        free(aux);
    }
}

struct Hash_Entry *Hashtable_SearchID(char *citizenID, struct Hash_Entry *p) {
    // TODO SO MANY FUCKING STUFF
    struct Hash_Entry *temp = p;

    while (temp != NULL) {
        if (strcmp(temp->citizenID, citizenID) == 0) {
            return temp;
        }
        temp = temp->next;
    }

    return NULL;
}