#ifndef FUNCTIONS
#define FUNCTIONS


typedef struct Hash_Entry *h;
typedef struct Skip_List_Node* s;

int set_tableSize(FILE *fp);

int levelCasino(int maxSize);

int datecmp(int day1, int month1, int year1, int day2, int month2, int year2);

int ageGroup(int age);

void GenerateTodayDate(int *day, int *month, int *year);

int crossData(struct Hash_Entry *entry, char *citizenID, char *name, char *surname, char *country, int age);
int sandwichDates(int d1, int m1, int y1, int d2, int m2, int y2, struct Skip_List_Node *t);

int sandwichDatesPRJ2(int d1, int m1, int y1, int d2, int m2, int y2,int rd,int rm, int ry);

int read_data(int fd, char **messageout, int bufferSize);

int write_data(int fd, char *message, int bufferSize);

int write_data_binary(int fd, char *message, int bufferSize, int length);

int check6Months(char *travel_date, int d, int m, int y);

void perror_exit(char* message);
#endif
