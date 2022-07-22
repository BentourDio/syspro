#ifndef DATERECORD
#define DATERECORD

struct DateRecord{
    int day;
    int month;
    int year;
    int vstatus; // vaccine status || 1 = NO || 0 = YES
};

void DateRecord_Init(struct DateRecord** d, int day, int month, int year,int vstatus);

void DateRecord_Delete(struct DateRecord* d);

#endif