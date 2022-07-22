/*
    Dionysios Mpentour 1115201300115

    This file acts the intermediary between the vaccineMonitor programm
    and the data structures of the programm.
*/

#include <dirent.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "app.h"
#include "countrydir_list.h"
#include "countrylist.h"
#include "functions.h"
#include "monitor.h"
#include "myhashtable.h"

void SigAddVaccRec(int signo){
    printf("Child:%d caught interrupt\n",getpid());
}

void Monitor_Assist_Init(struct Monitor_Assist *M, char *p_read, char *p_write,
        int bloomS, int bufferS) {
    M->p_read = (char *) malloc(sizeof (char) * (strlen(p_read) + 1));
    strcpy(M->p_read, p_read);
    M->p_write = (char *) malloc(sizeof (char) * (strlen(p_write) + 1));
    strcpy(M->p_write, p_write);

    M->bloomSize = bloomS;
    M->bufferSize = bufferS;
    M->fd_read = 0;
    M->fd_write = 0;
}

void MonitorSet_InputDir(struct Monitor_Assist *M,char *inputDir){
    M->inputDir = (char*) malloc (sizeof(char)*strlen(inputDir)+1);
    strcpy(M->inputDir,inputDir);
}


    void Monitor_OpenPipes(struct Monitor_Assist *M) {
  M->fd_read = open(M->p_read, O_RDONLY);

  if (M->fd_read < 0) {
    printf("Error while initializing: %s \n", M->p_read);
    exit(1);
  }

  M->fd_write = open(M->p_write, O_WRONLY);

  if (M->fd_write < 0) {
    printf("Error while initializing: %s \n", M->p_write);
    exit(1);
  }
}

void Monitor_Assist_Purge(struct Monitor_Assist M) {
    free(M.p_read);
    free(M.p_write);
    free(M.inputDir);
    Purge();

    // close(M.fd_read);
    // close(M.fd_write);
}

void Monitor_Receive_Countries(struct Monitor_Assist *M) {
    // printf("Monitor_Receive_Dirs %d\n", getpid());
    // int numC = 0;

    Import_Countries(M);
}

void Monitor_SetHashTable(char *inputdir) {
    SetHashTable(inputdir);
}

void Monitor_Load(char *inputdir, struct Monitor_Assist M) {
  App_LoadDir(inputdir,M);
}

void Monitor_Send_Viruses(struct Monitor_Assist M) {
    SendViruses(M);
}

void Monitor_Print() {
    Print(); // print everything from the data structures app.h
}

void Monitor_Application(struct Monitor_Assist M,char* inputDir) {
    //char buffer[2048];

    sigset_t set1,set2;

    sigfillset(&set1);
    sigemptyset(&set2);

    while (1) {
        char* message = NULL;

        if (read_data(M.fd_read, &message, M.bufferSize) == -5) {
            continue;
        }
        
        sigprocmask(SIG_SETMASK, &set1, NULL);  // disallow everything here!
        
        // block sig
        if (strcmp(message, "finish") == 0) {
            break;
        }
        //travelRequest citizenID date countryFrom countryTo virusName
        if (strncmp(message, "travelRequest", 13) == 0) {
            char *p1, *p2, *p3, *p4, *p5, *p6;
            printf("Child:%d received:\n%s\n",getpid(),message);
            p1 = strtok(message, " ");
            p2 = strtok(NULL, " ");
            p3 = strtok(NULL, " ");
            p4 = strtok(NULL, " ");
            p5 = strtok(NULL, " ");
            p6 = strtok(NULL, " ");

            //printf("%s %s %s %s %s  Child:%d\n",p2,p3,p4,p5,p6,getpid());
            if (p1 != NULL && p1 != NULL && p1 != NULL && p1 != NULL && p1 != NULL && p1 != NULL)
                Monitor_TravReq(p2, p3, p4, p5, p6, M);
        }

        if (strncmp(message, "searchVaccinationStatus", 23) == 0) {
            char *p1, *p2;

            p1 = strtok(message, " ");
            p2 = strtok(NULL, " ");

            if (p1 != NULL && p2 != NULL) {
                Monitor_SearchVaccRec(p2, M);
            }
        }

        free(message);
        
        sigprocmask(SIG_SETMASK, &set2, NULL);  // disallow everything here!
    }
}

void Monitor_TravReq(char* citizenID, char* date, char* countryFrom, char* countryTo, char* virusName, struct Monitor_Assist M) {
    int i = App_TravelRequest(citizenID, date, countryFrom, countryTo, virusName);
    //printf("Monitor_TravReq code is %d\n",i);
    //char buffer[1024];
    if (i == 1) {
        //sprintf(buffer, "REQUEST REJECTED – YOU ARE NOT VACCINATED");

        write_data(M.fd_write, "1", M.bufferSize);
    }
    if (i == 2) {
        //sprintf(buffer,"REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE");
        write_data(M.fd_write, "2", M.bufferSize);
    }
    if (i == 0) {
        //sprintf(buffer, "REQUEST ACCEPTED – HAPPY TRAVELS");
        write_data(M.fd_write, "0", M.bufferSize);
    }
}

void Monitor_SearchVaccRec(char *citizenID, struct Monitor_Assist M) {
    //first check if citizen exists in idTable
    char * array = App_SearchVaccRec(citizenID);
//    printf("Child %d:\n%s\n",getpid(), array);
    write_data(M.fd_write, array, M.bufferSize);
    //write_data(M.fd_write, "finish", M.bufferSize);
    //printf("child finished writing\n");

    free(array);
}