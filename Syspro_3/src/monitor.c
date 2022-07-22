/*
    Dionysios Mpentour 1115201300115

    This file acts the intermediary between the monitorServer programm
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
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>

#include "app.h"
#include "countrydir_list.h"
#include "countrylist.h"
#include "functions.h"
#include "monitor.h"
#include "myhashtable.h"
#include "ConsumersQueue.h"


struct ConsumerQueue* queue = NULL;//extern variable defined in ConsumersQueue.h line 23

void SigAddVaccRec(int signo) {
    printf("Child:%d caught interrupt\n", getpid());
}

void Monitor_Assist_Init(struct Monitor_Assist *M, int port, int numThreads, int socketSize, int cyclicSize, int bloomSize) {
    M->bloomSize = bloomSize;
    M->cyclicSize = cyclicSize;
    M->socketSize = socketSize;
    M->port = port;
    M->numThreads = numThreads;

    M->fd_read = 0;
    M->fd_write = 0;

    M->tids_array = malloc(sizeof (pthread_t) * numThreads);

    M->load_lock = malloc(sizeof (pthread_mutex_t));

    pthread_mutex_init(M->load_lock, 0);

    M->filter_lock = malloc(sizeof (pthread_mutex_t));

    pthread_mutex_init(M->filter_lock, 0);


    M->filter_cond = malloc(sizeof (pthread_cond_t));

    pthread_cond_init(M->filter_cond, 0);

    M->items_processed = malloc(sizeof(int));
    
    *M->items_processed = 0;
    
    ConsumerQueue_init(cyclicSize);
}

void MonitorSet_InputDir(struct Monitor_Assist *M, char *inputDir) {
    M->inputDir = (char*) malloc(sizeof (char)*strlen(inputDir) + 1);
    strcpy(M->inputDir, inputDir);
}

void Monitor_OpenSocket(struct Monitor_Assist *M) {
    int sock;

    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*) &server;
    struct hostent *rem;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror_exit("socket");
    }

    if ((rem = gethostbyname("localhost")) == NULL) {
        herror("gethostbyname");
        exit(1);
    }

    server.sin_family = AF_INET; /* Internet domain */
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(M->port); /* Server port */

    if (connect(sock, serverptr, sizeof (server)) < 0) {
        perror_exit("connect");
    }


    M->fd_read = sock;
    M->fd_write = sock;
}

void Monitor_Assist_Purge(struct Monitor_Assist M) {
    Purge();
    free(M.inputDir);
    close(M.fd_read);

    free(M.tids_array);

    ConsumerQueue_Purge();

    pthread_mutex_destroy(M.load_lock);
    free(M.load_lock);

    pthread_mutex_destroy(M.filter_lock);
    free(M.filter_lock);

    pthread_cond_destroy(M.filter_cond);
    free(M.filter_cond);
    
    free(M.items_processed);
}

void Monitor_Receive_Countries(struct Monitor_Assist *M) {
    // printf("Monitor_Receive_Dirs %d\n", getpid());
    // int numC = 0;

    Import_Countries(M);
}

void Monitor_SetHashTable() {
    SetHashTable();
}

void Monitor_Load(struct Monitor_Assist *M) {
    *(M->items_processed) = 0;
    int items_produced = App_LoadDir(M);
    //printf("[MONITOR LOAD] {items produced are %d}\n",items_produced);
    // wait for process to complete
    pthread_mutex_lock(M->filter_lock);
    while (*(M->items_processed) < items_produced) {
        
        pthread_cond_wait(M->filter_cond, M->filter_lock);
        //printf("[  INSIDE MUTEX     ] ITEMS PROCESSED ARE %d AND ITEMS PRODUCED %d\n", *(M->items_processed),items_produced);
    }
    pthread_mutex_unlock(M->filter_lock);
}

void Monitor_Send_Viruses(struct Monitor_Assist M) {
    SendViruses(&M);
}

void Monitor_Print() {
    Print(); // print everything from the data structures app.h
}

void Monitor_Application(struct Monitor_Assist *M) {
    //char buffer[2048];

    while (1) {
        char* message = NULL;

        if (read_data(M->fd_read, &message, M->socketSize) == -5) {
            continue;
        }

        // block sig
        if (strcmp(message, "finish") == 0) {
            break;
        }
        //travelRequest citizenID date countryFrom countryTo virusName
        if (strncmp(message, "travelRequest", 13) == 0) {
            char *p1, *p2, *p3, *p4, *p5, *p6;
            //            printf("Child:%d received:\n%s\n", getpid(), message);
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

        if (strncmp(message, "addVaccinationRecords", 21) == 0) {
            char *p1, *p2;
            //            printf("%s montitor\n",message);
            p1 = strtok(message, " ");
            p2 = strtok(NULL, " ");

            if (p1 != NULL && p2 != NULL) {
                *(M->items_processed) = 0;
                int items_produced = App_LoadSingleDir((*M), p2);
                pthread_mutex_lock(M->filter_lock);
                while (*(M->items_processed) < items_produced) {
                    
                    pthread_cond_wait(M->filter_cond, M->filter_lock);
                    //printf("[  INSIDE MUTEX     ] ITEMS PROCESSED ARE %d AND ITEMS PRODUCED %d\n", *(M->items_processed),items_produced);
                }
                pthread_mutex_unlock(M->filter_lock);
                
                SendViruses(M);
            }
        }

        if (strncmp(message, "exit", 5) == 0) {
            printf("Monitor:%5d shutting down gracefully\n", getpid());
            free(message);
            break;
        }
        free(message);
    }
}

void Monitor_TravReq(char* citizenID, char* date, char* countryFrom, char* countryTo, char* virusName, struct Monitor_Assist *M) {
    int i = App_TravelRequest(citizenID, date, countryFrom, countryTo, virusName);
    //printf("Monitor_TravReq code is %d\n",i);
    //char buffer[1024];
    if (i == 1) {
        //sprintf(buffer, "REQUEST REJECTED – YOU ARE NOT VACCINATED");

        write_data(M->fd_write, "1", M->socketSize);
    }
    if (i == 2) {
        //sprintf(buffer,"REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE");
        write_data(M->fd_write, "2", M->socketSize);
    }
    if (i == 0) {
        //sprintf(buffer, "REQUEST ACCEPTED – HAPPY TRAVELS");
        write_data(M->fd_write, "0", M->socketSize);
    }
}

void Monitor_SearchVaccRec(char *citizenID, struct Monitor_Assist *M) {
    //first check if citizen exists in idTable
    char * array = App_SearchVaccRec(citizenID);
    //    printf("Child %d:\n%s\n",getpid(), array);
    write_data(M->fd_write, array, M->socketSize);
    //write_data(M.fd_write, "finish", M.bufferSize);
    //printf("child finished writing\n");

    free(array);
}

void * MonitorThread_Main(void * argp) {
    struct Monitor_Assist *M = (struct Monitor_Assist *) argp;

    printf("I am the newly created thread %lu\n", pthread_self());

    char * pathFile;

    while (1) {
        pathFile = ConsumerQueue_Obtain();

        
        if (pathFile == NULL) {
            break;
        }
        //printf("Pathfile is %s\n",pathFile);


        FILE *fp = fopen(pathFile, "r");
        if (fp == NULL) {
            perror("Monitor_Load. fopen failed");
            continue;
        }
        //printf("About to enter Load_FIle as thread:%lu\n",pthread_self());
        if (M->load_lock != NULL) {
            pthread_mutex_lock(M->load_lock);
        }
        Load_File(fp, M->bloomSize, M->load_lock);
        if (M->load_lock != NULL) {
            pthread_mutex_unlock(M->load_lock);
        }
        
        //printf("Just exited Load_File as thread:%lu\n",pthread_self());
        fclose(fp);
        if(pathFile!= NULL)
            free(pathFile);
        
        pthread_mutex_lock(M->filter_lock);
        (*(M->items_processed))++;
        pthread_cond_signal(M->filter_cond);
        pthread_mutex_unlock(M->filter_lock);
    }

    printf("I am the newly created thread and about to exit! \n");
    return NULL;
}

void Monitor_CreateThreads(struct Monitor_Assist *M) {
    int i = 0;
    for (i = 0; i < M->numThreads; i++) {
        int err;

        err = pthread_create(&M->tids_array[i], 0, MonitorThread_Main, M);

        if (err == -1) {
            perror("pthread_create");
            exit(1);
        }
    }
}

void Monitor_DestroyThreads(struct Monitor_Assist *M) {
    int i = 0;

    queue->cancel = true;

    pthread_cond_broadcast(&queue->cond_nonempty);

    for (i = 0; i < M->numThreads; i++) {
        int err = pthread_join(M->tids_array[i], 0);

        if (err == -1) {
            perror("pthread_join");
            exit(1);
        }
    }
}