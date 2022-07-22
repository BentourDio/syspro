#ifndef MYLIST
#define MYLIST

typedef struct DateRecord *dateptr;
typedef struct Hash_Entry *hash_ptr;
typedef struct  Skip_List_Node* skip_ptr;

struct Disease_List {
  char *virusName;
  int vaccinated_sum;
  int not_vaccinated_sum;

  struct Skip_List_Node *vacc_head;
  struct Skip_List_Node *unvacc_head;

  struct Disease_List *next;
};



// Disease_List
// fucntions***********************************************************

struct Disease_List *Disease_List_Init(char *virusName);

void Disease_List_Insert(struct Disease_List **head, char *virusName,
                         struct Hash_Entry *hptr);

void Disease_List_Purge(struct Disease_List *head);

struct Disease_List *Disease_List_Search(struct Disease_List *head,
                                         char *virusName);

void Disease_List_Print(struct Disease_List *head);
// Skip_List
// functions*****************************************************************


#endif
