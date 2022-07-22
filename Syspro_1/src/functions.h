#ifndef FUNCTIONS
#define FUNCTIONS


typedef struct Hash_Entry *h;
typedef struct Skip_List_Node* s;

int set_tableSize(FILE *fp);

int levelCasino(int maxSize);

int datecmp(int day1, int month1, int year1, int day2, int month2, int year2);

int ageGroup(int age);

void GenerateTodayDate(int *day, int *month, int *year);

int crossData(struct Hash_Entry *entry, char *citizenID, char *name,
              char *surname, char *country, int age);
int sandwichDates(int d1, int m1, int y1, int d2, int m2, int y2,
                  struct Skip_List_Node *t);
#endif
