/*
    Dionysios Mpentour 1115201300115
    This file acts as the coordinator between the travelMonitor programm and its child forked
    process which act as the vaccineMonitors.
    All the pipes and file descriptors are initialized here and the respective data is written to the children.
    Also in this file the signal handler is located.
*/



#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "app.h"
#include "coordinator.h"
#include "countrydir_list.h"
#include "functions.h"
#include "mybloomfilter.h"
#include "mylist.h"
#include "pipelist.h"
#include "travelReq_list.h"
#include "monitor.h"

static struct CountryDirList *dir_ptr = NULL; // one node for each country
static struct Record_list *record_ptr = NULL; // one node for each pipe. 1st node is for read, 2nd for write etc
static struct Disease_List *dis_ptr = NULL; // two skiplists for each disease for accepted and rejected requests
static struct Monitor_Assist Coordinator_Assist;

static pid_t *pids_array = NULL; // one item for each process
static int accepted = 0;
static int rejected = 0;
static int quit_activated = 0;
static int numMon;

void Coordinator_catchinterrupt(int signo) {
    if (signo == 2 || signo == 3) {
        quit_activated = 1;
        Coordinator_LogFile();
    } else {
        //printf("I caught a SIGCHLD\n");
        pid_t pid;
        int status;
        while ((pid = waitpid(-1, &status, WNOHANG)) != -1) {
            OperationLazarusComeOutakaRespawn(pid);
        }
    }

}

void OperationLazarusComeOutakaRespawn(int deadpid) {
    printf("Oh my god a child has been brutally murdered. Oh well, time to make another. PS i can't.\n");

    /*
    struct CountryDirList *dir_temp = dir_ptr;
    struct Record_list *rec_temp = NULL;
    int flag =0; int i, argc =5;
    char *argv[6];

    argv[0] = "vaccineMonitor";
    argv[1] = "-p1";
    argv[2] = NULL;
    argv[3] = "-p2";
    argv[4] = NULL;
    argv[5] = NULL;

    while(dir_temp != NULL){
    
      if(dir_temp->pid == deadpid ){
        //close fds| i need to close them only once since multiple dirs maybe belong to 1 monitor
      
        if ( flag == 0 ){
          //close(dir_temp->input->fd);
          //close(dir_temp->output->fd);
          rec_temp = dir_temp->input;
          flag  = 1;
        }

        dir_temp->pid = -1;
      }
    
      dir_temp = dir_temp->next;
    }

    for (i=0; i<numMon;i++) {
      if (pids_array[i] == deadpid)
        break;
    }

    pids_array[i] = fork();
  
    char* pipename1 = rec_temp->pipeName;
    char* pipename2 = rec_temp->next->pipeName;
    printf("p1:%s p2:%s\n",pipename1,pipename2);

    if (pids_array[i] == 0){
      argv[2] = pipename2;
      argv[4] = pipename1;

      main_vaccine_monitor(argc, argv);
      exit(0);
    }

    int fd1 = open(pipename1, O_WRONLY);

    if (fd1 < 0) {
      printf("Error while initializing: %s \n", pipename1);
      exit(1);
    }

    int fd2 = open(pipename2, O_RDONLY);

    if (fd2 < 0) {
      printf("Error while initializing: %s \n", pipename2);
      exit(1);
    }
    rec_temp->fd = fd1;
    rec_temp->next->fd = fd2;


    //assign the pids to the countries in order to know which country is monitored by the new process
    dir_temp = dir_ptr;
  
    while(dir_temp != NULL){
    
      if(dir_temp->pid == -1){
        dir_temp->pid = pids_array[i];
      }
    
      dir_temp = dir_temp -> next;
    }

    int bloomSize = Coordinator_Assist.bloomSize;
    int bufferSize = Coordinator_Assist.bufferSize;
    char *inputDir = Coordinator_Assist.inputDir;

    write(rec_temp->fd, &bloomSize, sizeof(bloomSize));
    write(rec_temp->fd, &bufferSize, sizeof(bufferSize));
    write_data(rec_temp->fd, inputDir, bufferSize);
    printf("1\n");*/
}

void SetCoordinator_Assist(char* inputDir, int bloomSize, int bufferSize) {
    Monitor_Assist_Init(&Coordinator_Assist, "NULL", "NULL", bloomSize, bufferSize);
    MonitorSet_InputDir(&Coordinator_Assist, inputDir);
}

void Coordinator_LogFile() {
    char log_file[64];
    sprintf(log_file, "log_file.%d", getpid());
    FILE *fp = fopen(log_file, "w+");

    if (fp == NULL) {
        perror("fopen Coordinator_LogFile");
    }

    struct CountryDirList * dir_temp = dir_ptr;

    while (dir_temp != NULL) {
        fprintf(fp, "%s\n", dir_temp->country);

        dir_temp = dir_temp->next;
    }

    fprintf(fp, "TOTAL TRAVEL REQUESTS %d\nACCEPTED %d\nREJECTED %d\n", accepted + rejected, accepted, rejected);
}

int Load_Dir(char *inputDir) {
    struct dirent *direntp;
    int numDir = 0;

    DIR *drp = opendir(inputDir);

    if (drp == NULL) {
        printf("Directory could not be opened\n");
        exit(-1);
    }

    while ((direntp = readdir(drp)) != NULL) {
        if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
            continue;
        // printf("%s\n", direntp->d_name);
        // struct CountryDirList* l_temp = CountryDirList_Search(dir_list, di) no
        // neeed to search since countries dir are unique

        CountryDirList_Insert(&dir_ptr, direntp->d_name); // insert ordered
        numDir++;
    }

    closedir(drp);
    return numDir;
}

void Coordinator_Load_Pipes(int numMon) {
    // int pipes = numMon*2;

    printf("Coordinator started creating pipes...\n");

    char pipename[2048];
    // store pipenames in common list
    for (int i = 0; i < numMon * 2; i++) {
        sprintf(pipename, "monitor_%d.pipe", i);
        record_ptr = Create_Record(record_ptr, pipename);
    }
}

void Coordinator_Fork_Children(char *inputDir, int _numMon, int bufferSize, int bloomSize) {
    int i;
    char *argv[6];

    argv[0] = "vaccineMonitor";
    argv[1] = "-p1";
    argv[2] = NULL;
    argv[3] = "-p2";
    argv[4] = NULL;
    argv[5] = "mpampis";

    numMon = _numMon;
    pids_array = malloc(sizeof (pid_t) * numMon);

    struct Record_list *temprec = record_ptr;

    for (i = 0; i < numMon; i++) {
        printf("Parent is forking %d child \n", i);

        pids_array[i] = fork();

        char *pipename1 = temprec->pipeName;
        char *pipename2 = temprec->next->pipeName;

        if (pids_array[i] == 0) {
            argv[2] = pipename1;
            argv[4] = pipename2;
            //exec
            //int mainc(int, char**);
            //mainc(argc, argv);

            execl("./vaccineMonitor", argv[1], argv[2], argv[3], argv[4],argv[5],NULL);

            printf("wtf happened\n");
            exit(0);
        }

        int fd1 = open(pipename1, O_WRONLY);

        if (fd1 < 0) {
            printf("Error while initializing: %s \n", pipename1);
            exit(1);
        }

        int fd2 = open(pipename2, O_RDONLY);

        if (fd2 < 0) {
            printf("Error while initializing: %s \n", pipename2);
            exit(1);
        }

        temprec->fd = fd1;
        temprec->next->fd = fd2;

        struct CountryDirList *temp = dir_ptr;

        while (temp != NULL) {
            if (temp->input == temprec || temp->output == temprec) {
                temp->pid = pids_array[i];
            }
            temp = temp->next;
        }

        write(fd1, &bloomSize, sizeof (bloomSize));
        write(fd1, &bufferSize, sizeof (bufferSize));
        write_data(fd1, inputDir, bufferSize);

        temprec = temprec->next;
        temprec = temprec->next;
    }
}

void Coordinator_Send_Countries(int bufferSize) {
    struct CountryDirList *temp = dir_ptr;

    while (temp != NULL) {
        int fd = temp->input->fd;

        // printf("Sending %s to FD:%d, pipe:%s \n", temp->country, fd,
        //        temp->input->pipeName);

        write_data(fd, temp->country, bufferSize);
        temp = temp->next;
    }

    struct Record_list *temprec = record_ptr;

    while (temprec != NULL) {
        int fd = temprec->fd;
        if (write_data(fd, "finish", bufferSize) < 0) {
            perror("write_data on coordinator");
            exit(0);
        }

        temprec = temprec->next;
        temprec = temprec->next;
    }
}

void Coordinator_ReceiveVirusFromChild(struct CountryDirList* dir_temp, char* virusName, int bufferSize, int bloomSize) {
    int fd_read = dir_temp->output->fd;

    while (1) {

        char *message = NULL;
        read_data(fd_read, &message, bufferSize);

        printf("%s\n", message);
        if (strcmp(message, "finish") == 0) {
            free(message);
            break;
        } else {
            //read any new viruses and load updated bloomfilters
            printf("Parent %d - received virus:%s\n", getpid(), message);

            char* virus = message;

            struct Disease_List * dtemp = Disease_List_Search(dis_ptr, virus);

            if (dtemp == NULL) {
                Disease_List_Insert(&dis_ptr, virus, NULL, bloomSize);
            }

            free(message);

            //now read and load the bloomfilters
            read_data(fd_read, &message, bufferSize);

            int *filter = (int *) message;

            for (int i = 0; i < bloomSize; i++) {
                dtemp->bf->data[i] = dtemp->bf->data[i] | filter[i];
            }

            free(message);
        }
    }

}

void Coordinator_Receive_Viruses(int bufferSize, int bloomSize) {
    // printf("Monitor_Receive_Dirs %d\n", getpid());
    // int numC = 0;

    int numViruses = 0;

    struct Record_list *temprec = record_ptr;

    temprec = temprec->next;

    while (temprec != NULL) {
        int fd_read = temprec->fd;

        while (1) {
            //printf("\n");
            char *message = NULL;
            read_data(fd_read, &message, bufferSize);

            //  printf("%s\n",message);
            if (strcmp(message, "finish") == 0) {
                //printf("I finished with this read\n");
                free(message);
                break;
            } else {
                // insert to countries ...
                numViruses++;
                printf("Parent %d - received virus:%s\n", getpid(), message);
                char *virusName = message;

                struct Disease_List *dtemp = Disease_List_Search(dis_ptr, virusName);

                if (dtemp == NULL) {
                    Disease_List_Insert(&dis_ptr, virusName, NULL, bloomSize);
                    dtemp = Disease_List_Search(dis_ptr, virusName);
                }

                free(message);
                // read and load bloomfilter
                read_data(fd_read, &message, bufferSize);

                int *filter = (int *) message;

                for (int i = 0; i < bloomSize; i++) {
                    dtemp->bf->data[i] = dtemp->bf->data[i] | filter[i];
                }
                //printf("Parent:%d - received bloomfilter for virus:%s\n",getpid(),dtemp->virusName);
                free(message);
            }
        }

        if (temprec != NULL) {
            temprec = temprec->next;
        }
        if (temprec != NULL) {
            temprec = temprec->next;
        }
    }
}

void Coordinator_Receive_Viruses_Polling(int bufferSize, int bloomSize, int numMonitors) {
    struct Record_list *rec_temp = record_ptr;
    struct pollfd *fdarray = malloc(sizeof (struct pollfd) * numMon);
    int i, j;

     int numViruses = 0;
     
    rec_temp = record_ptr;

    rec_temp = rec_temp->next;

    for (i = 0; i < numMon; i++) {
        fdarray[i].fd = rec_temp->fd;
        fdarray[i].events = POLLIN;
        fdarray[i].revents = 0;

        //printf("fd:%d\n",fdarray[i].fd);

        if (rec_temp != NULL) {
            rec_temp = rec_temp->next;
        }
        if (rec_temp != NULL) {
            rec_temp = rec_temp->next;
        }
    }


    int collected = 0;

    while (collected < numMon) {
        printf("polling for results: %d of %d collected \n", collected, numMon);
        int rc = poll(fdarray, numMon, -1);

        if (rc == 0) {// poll will make indefinetely even if fds are not ready
            printf("Time out or poll 0 \n");
            break;
        }

        if (rc < 0) {// poll will make indefinetely even if fds are not ready
            perror("Poll < 0 \n");
            break;
        }

        for (j = 0; j < numMon; j++) {

            if ((((fdarray[j].revents) & POLLIN) != 0)) {
                int fd_read = fdarray[j].fd;


                while (1) {
                    //printf("\n");
                    char *message = NULL;
                    read_data(fd_read, &message, bufferSize);

                    //  printf("%s\n",message);
                    if (strcmp(message, "finish") == 0) {
                        //printf("I finished with this read\n");
                        free(message);
                        break;
                    } else {
                        // insert to countries ...
                        numViruses++;
                        printf("Parent %d - received virus:%s\n", getpid(), message);
                        char *virusName = message;

                        struct Disease_List *dtemp = Disease_List_Search(dis_ptr, virusName);

                        if (dtemp == NULL) {
                            Disease_List_Insert(&dis_ptr, virusName, NULL, bloomSize);
                            dtemp = Disease_List_Search(dis_ptr, virusName);
                        }

                        free(message);
                        // read and load bloomfilter
                        read_data(fd_read, &message, bufferSize);

                        int *filter = (int *) message;

                        for (int i = 0; i < bloomSize; i++) {
                            dtemp->bf->data[i] = dtemp->bf->data[i] | filter[i];
                        }
                        //printf("Parent:%d - received bloomfilter for virus:%s\n",getpid(),dtemp->virusName);
                        free(message);
                    }
                }

                collected++;
            }
        }
    }

    free(fdarray);
}

void Coordinator_Shutdown_Children(int numMon) {
    int i;
    for (i = 0; i < numMon; i++) {
        printf("Parent is killing %d child \n", i);
        kill(pids_array[i], SIGKILL);
    }

    for (i = 0; i < numMon; i++) {
        printf("Parent is waiting %d child \n", i);
        waitpid(pids_array[i], 0, 0);
    }

    free(pids_array);
}

void Coordinator_Create_Pipes() {
    Create_Pipes_From_List(record_ptr);
}

void Coordinator_Assign_Pipes() {
    struct CountryDirList *temp = dir_ptr;
    struct Record_list *temprec = record_ptr;

    while (temp != NULL) {
        temp->input = temprec;
        temprec = temprec->next;
        temp->output = temprec;
        temprec = temprec->next;
        temp = temp->next;

        if (temprec == NULL) {
            temprec = record_ptr;
        }
    }
}

void Coordinator_Destroy_Pipes() {
    Destroy_Pipes_From_List(record_ptr);
}

void Coordinator_Print() {

    CountryDirList_Print(dir_ptr);
    //Disease_List_Print(dis_ptr);
    /*struct Disease_List* dis_temp = dis_ptr;

    while(dis_temp != NULL){
    
      printf("Virus:%s accepted:%d rejected:%d\n",dis_temp->virusName,dis_temp->stats_head->acceptedNum,dis_temp->stats_head->rejectedNum);
    
      printf("Accepted\n");
      if (dis_temp->stats_head->accepted_list == NULL)
        printf("list empty\n");
      else
        TravelReqList_Print(dis_temp->stats_head->accepted_list);
    
      printf("rejected\n");
      if (dis_temp->stats_head->accepted_list == NULL)
        printf("list is empty\n");
      else
        TravelReqList_Print(dis_temp->stats_head->rejected_list);
    
      dis_temp = dis_temp->next;
    }*/
}

void Coordinator_Purge() {
    Disease_List_Purge(dis_ptr);
    CountryDirList_Purge(dir_ptr);
    Destroy_Record_list(record_ptr);
    Monitor_Assist_Purge(Coordinator_Assist);
}

void CoordinatorApplication(int bufferSize, int bloomSize) {

    char buffer[2048];
    char buffer2[2048];
    sigset_t set1, set2;
    sigfillset(&set1);
    sigemptyset(&set2);

    printf("Working with bufferSize: %d\n", bufferSize);
    printf("Working with bloomSize : %d\n", bloomSize);
    printf("Coordinator PID is     : %d\n", getpid());

    while (quit_activated == 0) {
        printf("-->");

        if (fgets(buffer2, sizeof (buffer), stdin) == NULL || quit_activated == 1) {
            if (errno == EINTR) {
                continue;
            }
            printf("fgets breaks\n");
            break;
        }

        sigprocmask(SIG_SETMASK, &set1, NULL);
        //disallow all signals here
        if (buffer2[strlen(buffer2) - 1] == '\n') {
            buffer2[strlen(buffer2) - 1] = '\0';
        }

        if (buffer2[0] == '/') {
            strcpy(buffer, buffer2);
        } else {
            strcpy(buffer, "/");
            strcat(buffer, buffer2);
        }

        if ((strcmp(buffer, "/print")) == 0) {
            Coordinator_Print();
        }
        if (strcmp(buffer, "/exit") == 0) {
            break;
        }
        // travelRequest citizenID date countryFrom countryTo virusName
        if (strncmp(buffer, "/travelRequest", 14) == 0) {
            // TODO
            char *p1, *p2, *p3, *p4, *p5, *p6;
            p1 = strtok(buffer, " \t");
            p2 = strtok(NULL, " \t");
            p3 = strtok(NULL, " \t");
            p4 = strtok(NULL, " \t");
            p5 = strtok(NULL, " \t");
            p6 = strtok(NULL, " \t");
            if (p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL && p5 != NULL &&
                    p6 != NULL) {
                TravelRequest(p2, p3, p4, p5, p6, bufferSize);
            }
        }

        // travelStats virusName date1 date2 [country]
        if (strncmp(buffer, "/travelStats", 12) == 0) {
            // TODO
            char *p1, *p2, *p3, *p4, *p5;
            p1 = strtok(buffer, " \t");
            p2 = strtok(NULL, " \t");
            p3 = strtok(NULL, " \t");
            p4 = strtok(NULL, " \t");
            p5 = strtok(NULL, " \t");
            if (p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL && p5 != NULL) {
                TravelStatsCountry(p2, p3, p4, p5);
            }
            if (p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL && p5 == NULL) {
                TravelStats(p2, p3, p4);
            }
        }
        // addVaccinationRecords country
        if (strncmp(buffer, "/addVaccinationRecords", 22) == 0) {
            // TODO
            char *p1, *p2;
            p1 = strtok(buffer, " \t");
            p2 = strtok(NULL, " \t");

            if (p1 != NULL && p2 != NULL) {
                AddVaccinationRecords(p2, bufferSize, bloomSize);
            }
        }
        // searchVaccinationStatus citizenID
        if (strncmp(buffer, "/searchVaccinationStatus", 23) == 0) {
            // TODO
            char *p1, *p2;
            p1 = strtok(buffer, " \t");
            p2 = strtok(NULL, " \t");

            if (p1 != NULL && p2 != NULL) {
                SearchVaccinationStatusPolling(p2, bufferSize, numMon);
            }
        }

        sigprocmask(SIG_SETMASK, &set2, NULL);
    } // end of while(1)
}

void TravelRequest(char *citizenID, char *date, char *countryFrom,
        char *countryTo, char *virusName, int bufferSize) {
    // printf("Travel-Request
    // %s|%s|%s|%s|%s\n",citizenID,date,countryFrom,countryTo,virusName);

    // check if virus exists
    struct Disease_List *dis_temp = Disease_List_Search(dis_ptr, virusName);

    if (dis_temp == NULL) {
        printf("[TravelStats] Virus:%s doesn't exist\n", virusName);
        return;
    }

    struct CountryDirList *dir_temp = CountryDirList_Search(dir_ptr, countryFrom);

    if (dir_temp == NULL) {
        printf("[TravelStats] Country:%s doesn't exist\n", countryFrom);
        return;
    }

    int success = 0;

    BloomFilter_Search(dis_temp->bf, citizenID, &success);

    if (success != 0) { // here i have checked coordinator's bloomfilter
        printf("REQUEST REJECTED – YOU ARE NOT VACCINATED\n");
        rejected++;
        if (dis_temp->stats_head != NULL) {
            TravelReqList_Insert(&(dis_temp->stats_head->rejected_list), citizenID, date, countryTo);
            dis_temp->stats_head->rejectedNum++;
        }
        // need to load to travelrequest list the rejected request
        // printf("rejected++\n");
    } else {
        Send_TravelRequest(dir_temp, citizenID, date, countryFrom, countryTo, virusName, bufferSize, &success);
        //after i load data to a structure that Coordinator_Assists me with travelstats
        if (success != 0) {
            if (dis_temp->stats_head != NULL) {
                rejected++;
                TravelReqList_Insert(&(dis_temp->stats_head->rejected_list), citizenID, date, countryTo);
                dis_temp->stats_head->rejectedNum++;
            }
        } else {
            if (dis_temp->stats_head != NULL) {
                accepted++;
                TravelReqList_Insert(&(dis_temp->stats_head->accepted_list), citizenID, date, countryTo);
                dis_temp->stats_head->acceptedNum++;
            }
        }
    }
}

void Send_TravelRequest(struct CountryDirList *dir_temp, char *citizenID,
        char *date, char *countryFrom, char *countryTo,
        char *virusName, int bufferSize, int *success) {

    char command[1024];

    sprintf(command, "travelRequest %s %s %s %s %s", citizenID, date, countryFrom,
            countryTo, virusName);

    // printf("Command to write:%s\n",command);

    // printf("Send_TravelRequest-->%s %s
    // %s\n",dis_temp->output->pipeName,dis_temp->input->pipeName,dis_temp->country);
    printf("Sending command:%s\nto child:%d\n", command, dir_temp->pid);
    if (write_data(dir_temp->input->fd, command, bufferSize) < 0) {
        perror("[Send_TravelRequest] write");
        exit(0);
    }
    char *message = NULL;
    read_data(dir_temp->output->fd, &message, bufferSize);

    if (atoi(message) == 0) {
        accepted++;
        printf("REQUEST ACCEPTED – HAPPY TRAVELS\n");
        (*success) = 0;
    }
    if (atoi(message) == 1) {
        rejected++;
        printf("REQUEST REJECTED – YOU ARE NOT VACCINATED\n");
        (*success) = 1;
    }
    if (atoi(message) == 2) {
        rejected++;
        printf("REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL "
                "DATE\n");
        (*success) = 1;
    }

    free(message);
}

/*
void Load_TravelStatsList(){
    printf("1\n");
    //printf("message:\n%s\nstatus:%d\n",message,code);
}*/

void TravelStats(char *virusName, char *date1, char *date2) {

    int d1, m1, y1, d2, m2, y2;
    int temp_accept = 0, temp_reject = 0;
    d1 = atoi(strtok(date1, "-"));
    m1 = atoi(strtok(NULL, "-"));
    y1 = atoi(strtok(NULL, " "));
    d2 = atoi(strtok(date2, "-"));
    m2 = atoi(strtok(NULL, "-"));
    y2 = atoi(strtok(NULL, "-"));

    //printf("Date1:%d|%d|%d date2:%d|%d|%d\n",d1,m1,y1,d2,m2,y2);
    //check if dates are legit first
    if (datecmp(d1, m1, y1, d2, m2, y2) >= 0) {
        printf("Date1 > Date2\n");
        return;
    }

    struct Disease_List *d_temp = Disease_List_Search(dis_ptr, virusName);

    if (d_temp == NULL) {
        printf("[ERROR] Virus doesn't exist in database or no second date was "
                "given\n");
        return;
    }
    struct TravelReqList* travel_temp = d_temp->stats_head->accepted_list;
    //tranverse accepted list
    while (travel_temp != NULL) {
        if (sandwichDatesPRJ2(d1, m1, y1, d2, m2, y2, travel_temp->day, travel_temp->month, travel_temp->year) == 0) {
            // sandwichdates checks if entry date is sandwiched between d1& d2
            temp_accept++;
        }
        travel_temp = travel_temp->next;
    }
    //tranverse rejected list
    travel_temp = d_temp->stats_head->rejected_list;
    while (travel_temp != NULL) {
        if (sandwichDatesPRJ2(d1, m1, y1, d2, m2, y2, travel_temp->day, travel_temp->month, travel_temp->year) == 0) {
            temp_reject++;
        }
        travel_temp = travel_temp->next;
    }

    printf("TOTAL REQUEST %d\nACCEPTED %d\nREJECTED %d\n", temp_accept + temp_reject, temp_accept, temp_reject);
}

void TravelStatsCountry(char *virusName, char *date1, char *date2, char *country) {
    int d1, m1, y1, d2, m2, y2;
    int temp_accept = 0, temp_reject = 0;
    d1 = atoi(strtok(date1, "-"));
    m1 = atoi(strtok(NULL, "-"));
    y1 = atoi(strtok(NULL, " "));
    d2 = atoi(strtok(date2, "-"));
    m2 = atoi(strtok(NULL, "-"));
    y2 = atoi(strtok(NULL, "-"));

    //printf("Date1:%d|%d|%d date2:%d|%d|%d\n",d1,m1,y1,d2,m2,y2);
    //check if dates are legit first
    if (datecmp(d1, m1, y1, d2, m2, y2) >= 0) {
        printf("Date1 > Date2\n");
        return;
    }

    struct Disease_List *d_temp = Disease_List_Search(dis_ptr, virusName);

    if (d_temp == NULL) {
        printf("[ERROR] Virus doesn't exist in database or no second date was "
                "given\n");
        return;
    }
    struct TravelReqList* travel_temp = d_temp->stats_head->accepted_list;
    //tranverse accepted list
    while (travel_temp != NULL) {
        if (sandwichDatesPRJ2(d1, m1, y1, d2, m2, y2, travel_temp->day, travel_temp->month, travel_temp->year) == 0) {
            // sandwichdates checks if entry date is sandwiched between d1& d2
            if (strcmp(country, travel_temp->countryTo) == 0) {
                temp_accept++;
            }
        }
        travel_temp = travel_temp->next;
    }
    //tranverse rejected list
    travel_temp = d_temp->stats_head->rejected_list;
    while (travel_temp != NULL) {
        if (sandwichDatesPRJ2(d1, m1, y1, d2, m2, y2, travel_temp->day, travel_temp->month, travel_temp->year) == 0) {
            if (strcmp(country, travel_temp->countryTo) == 0) {
                temp_reject++;
            }
        }
        travel_temp = travel_temp->next;
    }

    printf("TOTAL REQUEST %d\nACCEPTED %d\nREJECTED %d\n", temp_accept + temp_reject, temp_accept, temp_reject);
}

void AddVaccinationRecords(char *country, int bufferSize, int bloomSize) {

    struct CountryDirList* dir_temp = CountryDirList_Search(dir_ptr, country);

    if (dir_temp != NULL) {
        printf("sending SIGUSR1 to process:%d\n", dir_temp->pid);

        kill(dir_temp->pid, SIGUSR1);
        Coordinator_ReceiveVirusFromChild(dir_temp, country, bufferSize, bloomSize);
    } else {
        printf("Country and its subsequent directory do not exist\n");
    }


}

void SearchVaccinationStatus(char *citizenID, int bufferSize, int numMon) {

    struct Record_list *rec_temp = record_ptr;


    while (rec_temp != NULL) {
        char buffer[256];

        sprintf(buffer, "searchVaccinationStatus %s", citizenID);
        int out = rec_temp->fd;
        rec_temp = rec_temp->next;

        // printf("sending to child country:%s\n",dir_temp->country);
        if (write_data(out, buffer, bufferSize) < 0) {
            perror("[SearchVaccinationStatus] write_data coordinator");
            exit(0);
        }
        rec_temp = rec_temp->next;
    }

    rec_temp = record_ptr;

    rec_temp = rec_temp->next;

    while (rec_temp != NULL) {
        int fd_read = rec_temp->fd;
        char *message = NULL;

        read_data(fd_read, &message, bufferSize);

        if (strcmp(message, "negative") != 0) {
            printf("%s\n", message);
        }

        free(message);

        if (rec_temp != NULL) {
            rec_temp = rec_temp->next;
        }
        if (rec_temp != NULL) {
            rec_temp = rec_temp->next;
        }
    }
}

void SearchVaccinationStatusPolling(char* citizenID, int bufferSize, int numMon) {
    struct Record_list *rec_temp = record_ptr;
    int i = 0, j = 0;

    // send to all children
    while (rec_temp != NULL) {
        char buffer[256];

        sprintf(buffer, "searchVaccinationStatus %s", citizenID);
        int out = rec_temp->fd;
        rec_temp = rec_temp->next;

        // printf("sending to child country:%s\n",dir_temp->country);
        if (write_data(out, buffer, bufferSize) < 0) {
            perror("[SearchVaccinationStatus] write_data coordinator");
            exit(0);
        }
        rec_temp = rec_temp->next;
    }

    // receive data

    struct pollfd *fdarray = malloc(sizeof (struct pollfd) * numMon);
    rec_temp = record_ptr;

    rec_temp = rec_temp->next;

    for (i = 0; i < numMon; i++) {
        fdarray[i].fd = rec_temp->fd;
        fdarray[i].events = POLLIN;
        fdarray[i].revents = 0;

        //printf("fd:%d\n",fdarray[i].fd);

        if (rec_temp != NULL) {
            rec_temp = rec_temp->next;
        }
        if (rec_temp != NULL) {
            rec_temp = rec_temp->next;
        }
    }


    int collected = 0;

    while (collected < numMon) {
        //printf("polling for results: %d of %d collected \n", collected, numMon);
        int rc = poll(fdarray, numMon, -1);

        if (rc == 0) {// poll will make indefinetely even if fds are not ready
            printf("Time out or poll 0 \n");
            break;
        }

        if (rc < 0) {// poll will make indefinetely even if fds are not ready
            perror("Poll < 0 \n");
            break;
        }

        for (j = 0; j < numMon; j++) {
            char *message = NULL;

            if ((((fdarray[j].revents) & POLLIN) != 0)) {
                int fd_read = fdarray[j].fd;

                read_data(fd_read, &message, bufferSize);

                if (strcmp(message, "negative") != 0) {
                    printf("%s\n", message);
                }
                free(message);
                collected++;
            }
        }
    }

    free(fdarray);
}