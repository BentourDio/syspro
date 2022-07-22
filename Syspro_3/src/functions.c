/*
    Dionysios Mpentour 1115201300115
    
    Functions that are used often in the programm are stored here in order
    to make the code more readable. The functions routines are explained by
    their respective names
*/

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <error.h>

#include "date_record.h"
#include "functions.h"
#include "myhashtable.h"
#include "mylist.h"
#include "myskiplist.h"

// "${OUTPUT_PATH}" -m 4 -b 2000 -s 1000 -i data_small

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

int levelCasino(int maxSize) {// set skip list level
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
    else // if (strcmp(tmonth, "Dec") == 0)
        (*month) = 12;

    (*day) = atoi(num);
    (*year) = atoi(tyear);

    if (strcmp(time, "666") == 0 && strcmp(tday, "666") == 0) {
        printf("Devil's time\n");
        // just to get rid of unused variable warning
    }
}

int crossData(struct Hash_Entry *entry, char *citizenID, char *name,
        char *surname, char *country, int age) {
    if (strcmp(entry->citizenID, citizenID) != 0 ||
            strcmp(entry->name, name) != 0 || strcmp(entry->surname, surname) != 0 ||
            strcmp(entry->country, country) != 0 || entry->age != age) {
        // new entry has incorrect data
        return -1; // for incorrect data
    } else
        return 0; // for correct cross data
}

int sandwichDates(int d1, int m1, int y1, int d2, int m2, int y2,
        struct Skip_List_Node *t) {
    if (datecmp(d1, m1, y1, t->date_ptr->day, t->date_ptr->month,
            t->date_ptr->year) <= 0 &&
            datecmp(d2, m2, y2, t->date_ptr->day, t->date_ptr->month,
            t->date_ptr->year) >= 0) {
        return 0;
    } else {
        return -1;
    }
}

int sandwichDatesPRJ2(int d1, int m1, int y1, int d2, int m2, int y2, int rd, int rm, int ry) {
    if (datecmp(d1, m1, y1, rd, rm, ry) <= 0 &&
            datecmp(d2, m2, y2, rd, rm, ry) >= 0) {
        return 0;
    } else {
        return -1;
    }
}

int read_data(int fd, char **messageout, int bufferSize) { /* Read formated data*/
    int temp;
    int i = 0, length = 0, j, total;
    char *buffer = malloc(bufferSize + 1);
    int rest;
    buffer[bufferSize] = '\0';
    
    int r;

    if ((r=read(fd, &temp, sizeof (temp))) < 0) { /* Get length of string */
        if (errno != EINTR) {
            exit(-3);
        }
        printf("r = %d \n", r);
        return -5;
    }

    //     printf("read len = %d bufferSize=%d \n", temp, bufferSize);

    length = temp;
    total = length / bufferSize;

    *messageout = malloc(sizeof (char) * (length));

    memset(*messageout, 0, length);
    memset(buffer, 0, bufferSize);

    char *message = *messageout;

    // printf("read total=%d\n", total);

    for (j = 0; j < total; j++) {
        i = 0;
        while (i < bufferSize) /* Read $length chars */
            if (i < (i += (read(fd, &buffer[i], bufferSize - i))))
                return -3;
        memcpy(&message[j * bufferSize], buffer, i);
    }

    // printf("read rest\n");

    rest = length % bufferSize;
    if (rest != 0) {
        i = 0;
        while (i < rest) /* Read $length chars */
            if (i < (i += read(fd, &buffer[i], rest - i)))
                return -3;
        // buffer[length%bufferSize] = '\0';
        memcpy(&message[total * bufferSize], buffer, i);
    }

    //    printf("read message = %s\n", message);

    free(buffer);
    return length; /* Return size of string */
}

/*Write formatted data*/
int write_data(int fd, char *message, int bufferSize) {
    int total, j;
    int temp;
    int length = 0;
    length = strlen(message) + 1; /* Find length of string */
    temp = length;
    total = length / bufferSize;

    if (write(fd, &temp, sizeof (temp)) < 0) /* Send length first */
        return -2;

    // printf("wrote len = %d\n", temp);

    for (j = 0; j < total; j++) {
        if (write(fd, &message[j * bufferSize], bufferSize) < 0) { /* Send string */
            return -2;
        }
    }

    if (length % bufferSize != 0) {
        if (write(fd, &message[total * bufferSize], length % bufferSize) <
                0) { /* Send string */
            return -2;
        }
    }

    return length; /* Return size of string */
}

int write_data_binary(int fd, char *message, int bufferSize, int length) {
    int total, j;
    int temp;
    temp = length;
    total = length / bufferSize;

    if (write(fd, &temp, sizeof (temp)) < 0) /* Send length first */
        return -2;

    // printf("wrote len = %d\n", temp);

    for (j = 0; j < total; j++) {
        if (write(fd, &message[j * bufferSize], bufferSize) < 0) { /* Send string */
            return -2;
        }
    }

    if (length % bufferSize != 0) {
        if (write(fd, &message[total * bufferSize], length % bufferSize) <
                0) { /* Send string */
            return -2;
        }
    }

    return length; /* Return size of string */
}

int check6Months(char *travel_date, int d, int m, int y) {

    int travel_d, travel_m, travel_y;

    travel_d = atoi(strtok(travel_date, "-"));
    travel_m = atoi(strtok(NULL, "-"));
    travel_y = atoi(strtok(NULL, " "));

    int latest_d = d;
    int latest_m = m + 6;
    int latest_y = y;

    if (latest_m > 12) {
        latest_m = latest_m - 12;
        latest_y++;
    }
    //printf("%d.%d.%d    %d.%d.%d\n", travel_d, travel_m, travel_y, latest_d, latest_m, latest_y);

    if (datecmp(travel_d, travel_m, travel_y, d, m, y) < 0) {
        // traveldate is before vaccination date
        return -1;
    }

    if (datecmp(travel_d, travel_m, travel_y, d, m, y) > 0 &&
            datecmp(travel_d, travel_m, travel_y, latest_d, latest_m, latest_y) < 0) {
        // travel date is between vaccination date and expiration date
        return 0;
    }

    if (datecmp(travel_d, travel_m, travel_y, latest_d, latest_m, latest_y) > 0) {
        // traveldate is after expiration date. citizen needs to be vaccinated again
        return 1;
    }

    return -2; // something went wrong
}

void perror_exit(char* message){
    perror(message);
    exit(EXIT_FAILURE);
}