#ifndef TRAVELREQUEST_LIST_H
#define TRAVELREQUEST_LIST_H



struct Stats_Head{
    struct TravelReqList* accepted_list;
    struct TravelReqList* rejected_list;

    int acceptedNum;
    int rejectedNum;
};
/*
    every request is unique even if it has same information
    since it is a different request so all requests will go
    into their respective list
*/
struct TravelReqList{
    
    char* citizenID;
    int day;
    int month;
    int year;
    char* countryTo;

    struct TravelReqList* next;
};

void StatsHead_Init(struct Stats_Head** h);

struct TravelReqList* TravelReqList_Init(char* citizenID,char* date, char* countryTo);

void TravelReqList_Insert(struct TravelReqList** head, char* citizenID, char* date, char* countryTo);

void StatsHead_Purge(struct Stats_Head* h);

void TravelReqList_Purge(struct TravelReqList* h);

struct TravelReqList* TravelReqList_Search(struct TravelReqList* h,char* key);

void TravelReqList_Print(struct TravelReqList* h);
#endif