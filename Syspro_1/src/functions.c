#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "date_record.h"
#include "functions.h"
#include "myhashtable.h"
#include "mylist.h"
#include "myskiplist.h"

int set_tableSize(FILE *fp) {
    int lines = 1;
    int ch;

    while (!feof(fp)) {
        ch = fgetc(fp);

        if (ch == '\n') {
            lines++;
        }
    }
    // printf("THE TABLE SIZE IS %d\n", lines);
    return (lines / 2);
}

int levelCasino(int maxSize) {
    int level = 0;
    for (int i = maxSize - 1; i > 0; i--) {
        if ((rand() % 100) > 50)
            level++;
    }
    return level + 1;
}

int datecmp(int day1, int month1, int year1, int day2, int month2, int year2) {

    if (year1 != year2)
        return year1 - year2;

    if (month1 != month2)
        return month1 - month2;

    return day1 - day2;
}

int ageGroup(int age) {
    if (age >= 0 && age < 20) {
        return 1;
    }
    if (age >= 20 && age < 40) {
        return 2;
    }
    if (age >= 40 && age < 60) {
        return 3;
    }
    return 4;
}

void GenerateTodayDate(int *day, int *month, int *year) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char s[64];
    assert(strftime(s, sizeof (s), "%c", tm));
    char *tday, *tmonth, *num, *time, *tyear;
    tday = strtok(s, " ");
    tmonth = strtok(NULL, " ");
    num = strtok(NULL, " ");
    time = strtok(NULL, " ");
    tyear = strtok(NULL, " ");

    if (strcmp(tmonth, "Jan") == 0)
        (*month) = 1;
    else if (strcmp(tmonth, "Feb") == 0)
        (*month) = 2;
    else if (strcmp(tmonth, "Mar") == 0)
        (*month) = 3;
    else if (strcmp(tmonth, "Apr") == 0)
        (*month) = 4;
    else if (strcmp(tmonth, "May") == 0)
        (*month) = 5;
    else if (strcmp(tmonth, "Jun") == 0)
        (*month) = 6;
    else if (strcmp(tmonth, "Jul") == 0)
        (*month) = 7;
    else if (strcmp(tmonth, "Aug") == 0)
        (*month) = 8;
    else if (strcmp(tmonth, "Sep") == 0)
        (*month) = 9;
    else if (strcmp(tmonth, "Oct") == 0)
        (*month) = 10;
    else if (strcmp(tmonth, "Nov") == 0)
        (*month) = 11;
    else //if (strcmp(tmonth, "Dec") == 0)
        (*month) = 12;

    (*day) = atoi(num);
    (*year) = atoi(tyear);

    if(strcmp(time,"666")==0 && strcmp(tday, "666")==0){
        printf("Devil's time\n");
        //just to get rid of unused variable warning
    }
}

int crossData(struct Hash_Entry *entry, char *citizenID, char *name, char *surname, char *country, int age) {
    if (strcmp(entry->citizenID, citizenID) != 0 || strcmp(entry->name, name) != 0 ||
            strcmp(entry->surname, surname) != 0 ||
            strcmp(entry->country, country) != 0 || entry->age != age) {
        // new entry has incorrect data
        return -1; //for incorrect data
    } else
        return 0; //for correct cross data
}

int sandwichDates(int d1, int m1, int y1, int d2, int m2, int y2, struct Skip_List_Node* t) {
    if (datecmp(d1, m1, y1, t->date_ptr->day, t->date_ptr->month,
            t->date_ptr->year) <= 0 &&
            datecmp(d2, m2, y2, t->date_ptr->day, t->date_ptr->month,
            t->date_ptr->year) >= 0) {
        return 0;
    } else {
        return -1;
    }
}