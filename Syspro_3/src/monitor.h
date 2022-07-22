#ifndef MONITOR_H
#define MONITOR_H

#include "ConsumersQueue.h"

struct Monitor_Assist {
    char *inputDir;

    int fd_read;
    int fd_write;

    int sockfd;
    int numThreads;
    int socketSize;
    int cyclicSize;
    int port;

    int bloomSize;

    pthread_t * tids_array;

    pthread_mutex_t * load_lock;
    
    //struct ConsumerQueue * queue;


    pthread_mutex_t * filter_lock;
    pthread_cond_t * filter_cond;
    int * items_processed;
};

void SigAddVaccRec(int signo);

void Monitor_Assist_Init(struct Monitor_Assist *h, int port, int numThreads, int socketSize, int cyclicSize, int bloomSize);

void MonitorSet_InputDir(struct Monitor_Assist *M, char *inputDir);

void Monitor_OpenSocket(struct Monitor_Assist *M);

void Monitor_CreateThreads(struct Monitor_Assist *M);

void Monitor_DestroyThreads(struct Monitor_Assist *M);

void Monitor_Assist_Purge(struct Monitor_Assist M);

void Monitor_Receive_Countries(struct Monitor_Assist *M);

void Monitor_SetHashTable();

void Monitor_Load(struct Monitor_Assist*);

void Monitor_Print();

void Monitor_Send_Viruses(struct Monitor_Assist M);

void Monitor_Application(struct Monitor_Assist *M);

void Monitor_TravReq(char* citizenID, char* date, char* countryFrom, char* countryTo, char* virusName, struct Monitor_Assist *M);

void Monitor_SearchVaccRec(char* citizenID, struct Monitor_Assist *M);

int main_server(int argc, char *argv[]);

void Monitor_SetCountryList(char*);

void SetAssistAddVac(struct Monitor_Assist*);
#endif