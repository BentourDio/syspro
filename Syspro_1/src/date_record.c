#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "date_record.h"

void DateRecord_Init(struct DateRecord **d, int day, int month, int year,int vstatus){
    (*d) = (struct DateRecord*) malloc (sizeof(struct DateRecord));
    (*d)->day = day;
    (*d)->month = month;
    (*d)->year = year;
    (*d)->vstatus = vstatus;
}

void DateRecord_Delete(struct DateRecord *d){
    free(d);
}