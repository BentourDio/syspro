#ifndef MYHASHTABLE
#define MYHASHTABLE

struct Hashtable {
    int size;
    struct Hash_Entry **htable;
};

struct Hash_Entry {
    char *citizenID;
    char *name;
    char *surname;
    char *country;
    char *virusName;
    int age;

    int vstatus; // YES|NO
    int day, month, year; // date of vaccination

    struct Hash_Entry *next;
};

struct Hashtable Hashtable_Creation(int size);

int Hash_key(char *name, int size);

void Hash_Entry_Init(struct Hash_Entry **entry, char *citizenID, char *name,
        char *surname, char *country, char *virusName, int age);

int Htable_Insert(struct Hashtable h, char *citizenID, char *name,
        char *surname, char *country, char *virusName, int age);

void Hashtable_Print(struct Hashtable h);

void Hash_Lists_Print(struct Hash_Entry *p, int pos);

void Hashtable_Purge(struct Hashtable h);

void Hash_List_Purge(struct Hash_Entry* p);

struct Hash_Entry *Hashtable_SearchID(char *citizenID, struct Hash_Entry *p);
#endif
