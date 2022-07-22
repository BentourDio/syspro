#ifndef COUNTRYLIST
#define COUNTRYLIST


struct CountryList {

  char *path;
  char *country;
  char* filename;
  struct CountryList *filelist;
  struct CountryList *next;
};

void CountryList_Init(struct CountryList **newCNode, char *country);

void CountryList_InitF(struct CountryList **,char* filename);

struct CountryList* CountryList_InsertFile(struct CountryList **, char* filename);

struct CountryList *CountryList_Insert(struct CountryList **head, char *country); // returns inserted node


void CountryList_Print(struct CountryList *head);

void CountryList_PrintFiles(struct CountryList* head);

void CountryList_Purge(struct CountryList *head);

void groupIncrement(struct CountryList **node, int age);

void nonvaccIncrement(struct CountryList **node, int age);

void overallIncrement(struct CountryList **node, int age);

struct CountryList *CountryList_Search(struct CountryList *head, char *country);


struct CountryList* CountryList_SearchPath(struct CountryList* head, char* key);
struct CountryList* CountryList_SearchFile(struct CountryList* head, char* key);

void PrintPopStatusAgeVirusDates(struct CountryList *head);

#endif