#ifndef COUNTRYLIST
#define COUNTRYLIST


struct CountryList {

  int vacc_summ;
  int vacc_summ_overall;
  int unvacc_summ;

  int agegroup1; // 0-19
  int agegroup2; // 20-39
  int agegroup3; // 40-59
  int agegroup4; // 60+

  int agegroup1_unvacc;
  int agegroup2_unvacc;
  int agegroup3_unvacc;
  int agegroup4_unvacc;

  int overall1;
  int overall2;
  int overall3;
  int overall4;

  char *country;
  struct CountryList *filelist;
  struct CountryList *next;
};

void CountryList_Init(struct CountryList **newCNode, char *country);

struct CountryList *CountryList_Insert(struct CountryList **head,
                                       char *country); // returns inserted node

void CountryList_Print(struct CountryList *head);

void CountryList_Purge(struct CountryList *head);

void groupIncrement(struct CountryList **node, int age);

void nonvaccIncrement(struct CountryList **node, int age);

void overallIncrement(struct CountryList **node, int age);

struct CountryList *CountryList_Search(struct CountryList *head, char *country);

void PrintPopStatusAgeVirusDates(struct CountryList *head);

#endif