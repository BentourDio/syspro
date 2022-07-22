#ifndef MONITOR_H
#define MONITOR_H

struct Monitor_Assist{
    char* p_read;
    char *p_write;
    char *inputDir;
    int fd_read;
    int fd_write;
    int bloomSize;
    int bufferSize;
};

void SigAddVaccRec(int signo);

void Monitor_Assist_Init(struct Monitor_Assist *h, char *p_read, char *p_write,int bloomS,int bufferS);

void MonitorSet_InputDir(struct Monitor_Assist *M, char *inputDir);

void Monitor_OpenPipes(struct Monitor_Assist *M);

void Monitor_Assist_Purge(struct Monitor_Assist M);

void Monitor_Receive_Countries(struct Monitor_Assist *M);

void Monitor_SetHashTable(char * inputdir);

void Monitor_Load(char * inputdir, struct Monitor_Assist);

void Monitor_Print();

void Monitor_Send_Viruses(struct Monitor_Assist M);

void Monitor_Application(struct Monitor_Assist M,char* inputDir);

void Monitor_TravReq(char* citizenID, char* date, char* countryFrom, char* countryTo,char* virusName, struct Monitor_Assist M);

void Monitor_SearchVaccRec(char* citizenID, struct Monitor_Assist M);
#endif