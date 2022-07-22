#ifndef MYSKIPPLIST
#define SKIPLISt

typedef struct DateRecord *dateptr;
typedef struct Hash_Entry *hash_ptr;

struct Skip_List_Node {
  int citizenID;
  int maxSize;

  struct Skip_List_Node **level; // array for skip list levels
  struct Hash_Entry *info_ptr;   // pointer to information about citizen
  struct DateRecord *date_ptr;   // pointer about date and vaccination status
};

struct Skip_List_Node *Skip_List_Node_Init(int citizenID, int day, int month,
                                           int year, int vstatus);

struct Skip_List_Node *Skip_List_Insert(struct Skip_List_Node *head,
                                        int citizenID, int day, int month,
                                        int year, int vstatus,
                                        struct Hash_Entry *hptr);

struct Skip_List_Node *Skip_List_Search(struct Skip_List_Node *head,
                                        int citizenID);

void Skip_List_Delete(struct Skip_List_Node **head, int citizenID);

void Skip_List_Purge(struct Skip_List_Node *head);

void Print_Skip_List(struct Skip_List_Node *head);

void Skip_List_Print_Level(struct Skip_List_Node *head);

void Test_Sequence_Skip_List(struct Skip_List_Node *head);

#endif