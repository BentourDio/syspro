/*
    Dionysios Mpentour 1115201300115

    This file countains functions from Project 1 and Project2. It is used exclusively
    from the Monitor processes of the Project3.
 */


#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <netinet/in.h>
#include <pthread.h>

#include "app.h"
#include "countrylist.h"
#include "date_record.h"
#include "functions.h"
#include "mybloomfilter.h"
#include "myhashtable.h"
#include "mylist.h"
#include "myskiplist.h"
#include "ConsumersQueue.h"


//#define  _POSIX_C_SOURCE 200809L
// 666 John Papadopoulos Greece 52 COVID-19 YES 27-12-2020
// 666 John Papadopoulos Greece 52 Η1Ν1 ΝΟ

static struct Hashtable idTable;
static struct Disease_List *dis_ptr = NULL;
static struct CountryList *country_ptr = NULL; // this holds the directories and its directory's files
static int numCountries = 0;
static int accepted = 0;
static int rejected = 0;
//struct Monitor_Assist * AddVac = NULL;

void Create_Htable(int hsize) {

    //int hsize = set_tableSize(fp);

    idTable = Hashtable_Creation(hsize);
}

void Load_File(FILE *fp, int bloomSize, pthread_mutex_t * lock) {
    //printf("Application_Load_File. Monitor with pid:%d\n", getpid());

    
    fseek(fp, 0, SEEK_SET);
    int counter = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
  
    while ((read = getline(&line, &len, fp)) != -1) {
        counter++;
        char *name, *surname, *country, *age, *virusName, *date, *vacc_status,
                *citizenID;
        int iday = 0, imonth = 0, iyear = 0, vstatus;
        char *day, *month, *year;


        //printf("%s\n",line);
        /*
        in this section I take a line from the file and proccess it accordingly, as
        to get the information for each entry
         */
        citizenID = strtok(line, " ");
        name = strtok(NULL, " ");
        surname = strtok(NULL, " ");
        country = strtok(NULL, " ");
        age = strtok(NULL, " ");
        virusName = strtok(NULL, " ");
        vacc_status = strtok(NULL, " ");

        if (strncmp(vacc_status, "YES", 3) == 0) {

            vstatus = 0; // flag which means citizen is vaccined for specified illness

            date = strtok(NULL, " ");

            if (date == NULL) {
                printf("[ERROR] Entry with id: %s is YES but no date is given\n", citizenID);
                continue;
            }

            date[strcspn(date, "\n")] = 0;

            day = strtok(date, "-");
            month = strtok(NULL, "-");
            year = strtok(NULL, " ");
            iday = atoi(day);
            imonth = atoi(month);
            iyear = atoi(year);

        } else {
            date = strtok(NULL, " ");
            if (date != NULL) {
                printf("[ERROR] Entry with id: %s is NO but date is given\n", citizenID);
                continue;
            }
            date = "0";
            vstatus = 1; // citizen is not vaccined for specified virus
            vacc_status[strcspn(vacc_status, "\n")] = 0;
        }
        // this is a sample printf to highlight that the proccessing of the file is
        // done correctly
        /*printf("id:%s|name:%s|surname:%s|country:%s|age:%d|virusname:%s|vstatus:%d|"
               "day:%d|month:%d|year%d\n",
               citizenID, name, surname, country, atoi(age), virusName, vstatus,
               iday, imonth, iyear);*/

        // printf("inserting %s\n", citizenID);
        /*
          Now i start inserting the data to the structures. First is the
          HashTable aka idTable which holds all the information of each entry
          except the virus & vaccination info which is held in skip lists.
          First I search the idTable to determine whether the entry already exists
          there. If it doesn't htemp (hashtable teamp) = NULL and i insert in the
          idTable. I then search for the entry to hold it's position in the idTable
          for future use. If htemp!=NULL then i have a found an entry in the data
          structures so again we have two cases: case1: the entry exists with the
          same key but the other info are different so we decline this entry and
          continue the loop. case2: the entry exists with correct info and i hold
          its position at htemp pointer
         */
       // printf("[LOAD_FILE] before locking mutex as thread:%lu\n",pthread_self());
        //if (lock != NULL) {
        //    pthread_mutex_lock(lock);
      //  }

        //printf("ENTERED CRITICAL SECTION OF LOAD as thread:%lu\n",pthread_self());
        struct Hash_Entry *htemp = Hashtable_SearchID(citizenID, idTable.htable[Hash_key(citizenID, idTable.size)]);

        if (htemp == NULL) {
            int f = Htable_Insert(idTable, citizenID, name, surname, country,
                    virusName, atoi(age));

            if (f == -1) {
                printf("### register failed for id: %s  (error) ", citizenID);
              //  if (lock != NULL) {
          //          pthread_mutex_unlock(lock);
               // }
                printf("wtf\n");
                continue;
            }
            /*
            at this point an entry has surely been made, so i update country simply
            linked list which holds information about the population and the age
            groups of that population
            The country list is updated only when a new citizen registers
             */
            // Load_CountryList(country, atoi(age));

            htemp = Hashtable_SearchID(citizenID, idTable.htable[Hash_key(citizenID, idTable.size)]); // pointer to position of
          //  printf("1\n");
        } else {
            if (crossData(htemp, citizenID, name, surname, country, atoi(age)) == -1) {
                printf("invalid data for citizen\n");
                printf("[Warning] citizenID for id: %s (already exists)\n", htemp->citizenID);
            //    pthread_mutex_unlock(lock);
                printf("2\n");
                continue;
            }
        }

        /*
          Now i insert data in the skip lists structures. I check if a specific
          virus already exists in the simply linked disease list and if it doesn't i
          create the node and link it. If it does i simply update it's vaccinated or
          unvaccinated skip list.
          Each skip list node points to a hashtable entry, whose position is held
          on htemp
         */
        struct Disease_List *dtemp = Disease_List_Search(dis_ptr, virusName);
       // printf("182\n");
        if (dtemp == NULL) {
            Disease_List_Insert(&dis_ptr, virusName, htemp, bloomSize);
            dtemp = Disease_List_Search(dis_ptr, virusName);
        }

        if (vstatus == 1) {
            dtemp->unvacc_head = Skip_List_Insert(dtemp->unvacc_head, atoi(citizenID), iday, imonth,iyear, vstatus, htemp);
        }
        if (vstatus == 0) {
            dtemp->vacc_head = Skip_List_Insert(dtemp->vacc_head, atoi(citizenID), iday, imonth, iyear, vstatus, htemp);
            BloomFilter_Insert(dtemp->bf, citizenID);
        }
       // printf("198\n");
       // printf("EXITED CRITICAL SECTION OF LOAD as thread:%lu\n",pthread_self());
        if (lock != NULL) {
            //pthread_mutex_unlock(lock);
        }
    }

    if (line) {
        free(line);
    }
}

void StatusBloom(char* citizenID, char* virusName) {
    struct Hash_Entry *hash_temp = Hashtable_SearchID(citizenID, idTable.htable[Hash_key(citizenID, idTable.size)]);

    if (hash_temp == NULL) {
        printf("[Citizen not registered in database]\n");
        return;
    }

    struct Disease_List *temp = Disease_List_Search(dis_ptr, virusName);
    // find the skip list node
    if (temp == NULL) {
        printf("[Virus not registered in database: %s]\n", virusName);
        return;
    }

    int success = -1;

    BloomFilter_Search(temp->bf, citizenID, &success);

    if (success == 0) {
        printf("Citizen with id: %s MAYBE VACCINATED\n", citizenID);
        return;
    } else {
        printf("Citizen with id: %s NOT VACCINATED\n", citizenID);
        return;
    }
}

void VaccineStatus(char *citizenID, char *virusName) {
    // check if citizen exists in database
    struct Hash_Entry *hash_temp = Hashtable_SearchID(citizenID, idTable.htable[Hash_key(citizenID, idTable.size)]);

    if (hash_temp == NULL) {
        printf("[Citizen not registered in database]\n");
        return;
    }
    // find the virus
    struct Disease_List *temp = Disease_List_Search(dis_ptr, virusName);
    // find the skip list node
    if (temp == NULL) {
        printf("[Virus not registered in database: %s]\n", virusName);
        return;
    }
    struct Skip_List_Node *skip_temp =
            Skip_List_Search(temp->vacc_head, atoi(citizenID));

    if (skip_temp != NULL && skip_temp->date_ptr->vstatus == 0) {
        printf("VACCINATED ON %d-%d-%d", skip_temp->date_ptr->day,
                skip_temp->date_ptr->month, skip_temp->date_ptr->year);
    } else {
        printf("NOT VACCINATED\n");
    }
}

char* VaccineStatusCitizen(char *citizenID) {
    char line[1000];
    // check if citizenID exists
    struct Hash_Entry *hash_temp = Hashtable_SearchID(
            citizenID, idTable.htable[Hash_key(citizenID, idTable.size)]);

    char * array = malloc(10000);
    strcpy(array, "");

    if (hash_temp == NULL) {
        //        printf("Child:%d [Citizen not registered in database]\n", getpid());
        sprintf(line, "negative");
        strcat(array, line);
        return array;
    }



    // find the virus
    struct Disease_List *d_temp = dis_ptr;
    // transend skip lists and if citizenID found print data
    //printf("Child:%d [VaccinationStatus]\n", getpid());
    char credential[128];
    sprintf(credential, "%s %s %s %s\nAge %d\n", hash_temp->citizenID, hash_temp->name, hash_temp->surname, hash_temp->country, hash_temp->age);
    strcat(array, credential);

    while (d_temp != NULL) {
        struct Skip_List_Node *skip_temp;
        // transcend vaccinated skip list
        skip_temp = Skip_List_Search(d_temp->vacc_head, atoi(citizenID));
        if (skip_temp != NULL) {
            if (skip_temp->date_ptr->vstatus == 0) {
                sprintf(line, "%s VACCINATED ON %d-%d-%d\n", d_temp->virusName, skip_temp->date_ptr->day,
                        skip_temp->date_ptr->month, skip_temp->date_ptr->year);
                strcat(array, line);
            }
        }
        // search non vaccinated skip list for citizenID
        skip_temp = Skip_List_Search(d_temp->unvacc_head, atoi(citizenID));

        if (skip_temp != NULL) {
            if (skip_temp->date_ptr->vstatus == 1) {
                sprintf(line, "%s NOT YET VACCINATED\n", d_temp->virusName);
                strcat(array, line);
            }
        }
        d_temp = d_temp->next;
    }
    //printf("%s\n",array);
    return array;
}

void Purge() {
    Hashtable_Purge(idTable);

    Disease_List_Purge(dis_ptr);

    CountryList_Purge(country_ptr);
}
// *******************************************************************************************

//********************************************************************************************

void Print() {

    printf("\t\t\t\t\t\t\t#### This is Monitor:%d\n", getpid());
    //Hashtable_Print(idTable);
    /*struct CountryList * temp = country_ptr;
    
    temp = country_ptr;

    while(temp!=NULL){
        //printf("mpainw edw\n");
        CountryList_PrintFiles(temp->filelist);
        //if(temp->filelist ==  NULL)
            //printf("emptry you dork\n");
        temp=temp->next;
    }*/

}

//********************************************************************************************

void App_WriteLogFile() {
    char logfile[64];
    sprintf(logfile, "log_file.%d", getpid());
    FILE* fp = fopen(logfile, "w+");
    if (fp == NULL) {
        perror("logfile fopen");
    }

    struct CountryList* c_temp = country_ptr;

    while (c_temp != NULL) {
        fprintf(fp, "%s\n", c_temp->country);

        c_temp = c_temp -> next;
    }
    fprintf(fp, "TOTAL TRAVEL REQUESTS %d\nACCEPTED %d\nREJECTED %d", accepted + rejected, accepted, rejected);

    fclose(fp);
}

void Import_Countries(struct Monitor_Assist *M) {
    // printf("Monitor_Receive_Dirs %d\n", getpid());
    // int numC = 0;

    numCountries = 0;

    while (1) {
        printf("\n");
        char *message = NULL;
        read_data(M->fd_read, &message, M->socketSize);

        //  printf("%s\n",message);
        if (strcmp(message, "finish") == 0) {
            break;
        } else {
            // insert to countries ...
            numCountries++;
            //printf("Child %d [%s,%s,%d,%d] received:%s\n", getpid(), M->p_read,
            //        M->p_write, M->bloomSize, M->bufferSize, message);

            CountryList_Insert(&country_ptr, message);
        }
    }
}

void SetHashTable() {
    // function to find the appropriate size of IDtable
    struct dirent *direntp;
    char path[PATH_MAX];
    // create the path to the file
    strcpy(path, country_ptr->path);
    strcat(path, "/");

    //printf("path: %s\n", path);

    DIR *drp = opendir(path);
    // printf("SetHashtable:%s\n",country_list->country);
    if (drp == NULL) {
        printf("Monitor_SetHashTable. Directory %s could not be opened\n", country_ptr->country);
        exit(-1);
    }

    FILE *fp = NULL;
    int flag = 0;
    int fileNUM = 0;
    int lineNum, hsize;

    while ((direntp = readdir(drp)) != NULL) {
        if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
            continue;

        fileNUM++;

        if (flag == 0) {
            strcat(path, "/");
            strcat(path, direntp->d_name);
            fp = fopen(path, "r");

            if (fp == NULL) {
                perror("fopen failed");
                exit(-1);
            }

            lineNum = set_tableSize(fp); // this number is divided by 2 already
            fclose(fp);
            flag = 1;
        }
    }
    hsize = lineNum * fileNUM * numCountries;

    printf("Hsize: %d \n", hsize);
    Create_Htable(hsize);
    closedir(drp);
}

int App_LoadDir(struct Monitor_Assist *M) {
    struct dirent *direntp;
    struct CountryList *c_temp = country_ptr;

    int items_produced = 0;

    // char* input = (char* ) malloc (sizeof (char)* strcspn(c_temp->path, "/"))

    while (c_temp != NULL) {
        char path[PATH_MAX];
        strcpy(path, c_temp->path);

        //printf("Monitor_Load. Path:%s\n", path);

        DIR *drp = opendir(path);

        if (drp == NULL) {
            printf("Monitor_load. Directory %s could not be opened\n", path);
            exit(-1);
        }
        while ((direntp = readdir(drp)) != NULL) {
            if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
                continue;
            struct CountryList* filelist_temp = CountryList_SearchFile(country_ptr->filelist, direntp->d_name);

            if (filelist_temp != NULL) {
                continue;
            } else {
                CountryList_InsertFile(&(country_ptr->filelist), direntp->d_name);
            }

            char * pathFile = malloc(PATH_MAX * sizeof (char));

            strcpy(pathFile, path);
            strcat(pathFile, "/");
            strcat(pathFile, direntp->d_name);

            ConsumerQueue_Place(pathFile);// placing in the queue a pathfile for threads to pick and load into the
                                          // structures

            items_produced++;
        }


        c_temp = c_temp->next;
        closedir(drp);
    }

    return items_produced;
}

int App_LoadSingleDir(struct Monitor_Assist M, char* country) {
    int items_produced = 0;
    //printf("480\n");
    struct CountryList* country_temp = CountryList_Search(country_ptr, country);


    char path[PATH_MAX];
    strcpy(path, country_temp->path);
    printf("The path is %s\n", path);

    struct dirent *direntp;
    DIR *drp = opendir(path);

    if (drp == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    while ((direntp = readdir(drp)) != NULL) {
        if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0) {
            continue;
        }
        struct CountryList* filelist_temp = CountryList_SearchFile(country_ptr->filelist, direntp->d_name);

        if (filelist_temp != NULL) {
            continue;
        } else {
            CountryList_InsertFile(&(country_ptr->filelist), direntp->d_name);
        }

        char * pathFile = malloc(PATH_MAX * sizeof (char));
        strcpy(pathFile, path);
        strcat(pathFile, "/");
        strcat(pathFile, direntp->d_name);
        printf("pathfile is %s\n", pathFile);

        ConsumerQueue_Place(pathFile); //placing pathfile string into queue
        items_produced++;
    }
    closedir(drp);
    return items_produced;
}

void SendViruses(struct Monitor_Assist *M) {
    printf("Did i just enter the send Viruses\n");
    struct Disease_List *temp = dis_ptr;
    int bufferSize = M->socketSize;
    int fd = M->fd_write;

    while (temp != NULL) {
        printf("Sending %s to FD:%d \n", temp->virusName, fd);

        if (write_data(fd, temp->virusName, bufferSize) < 0) {
            perror("write, sendviruses");
        }
        if (write_data_binary(fd, (char*) temp->bf->data, bufferSize, M->bloomSize * sizeof (int)) < 0) {
            perror("write, sendviruses");
        }


        temp = temp->next;
    }

    if (write_data(fd, "finish", bufferSize) < 0) {
        perror("write_data on coordinator");
        exit(0);
    }
}

int App_TravelRequest(char* citizenID, char* date, char* countryFrom, char* countryTo, char* virusName) {

    struct Hash_Entry* h_temp = Hashtable_SearchID(citizenID, idTable.htable[Hash_key(citizenID, idTable.size)]);

    if (h_temp == NULL) {
        //citizen doesn't exist in database, don't bother searching in bloomfilter
        return 1;
    }

    int success = 0;

    struct Disease_List* dis_temp = Disease_List_Search(dis_ptr, virusName);

    BloomFilter_Search(dis_temp->bf, citizenID, &success);

    if (success != 0) {
        return 1; //citizen not vaccinated
    }
    //check unvaccinated persons skip list
    struct Skip_List_Node* skip_temp = Skip_List_Search(dis_temp->unvacc_head, atoi(citizenID));

    if (skip_temp != NULL) {
        return 1;
        //citizen not vaccinated
    }
    //check vaccinated persons skip list
    skip_temp = Skip_List_Search(dis_temp->vacc_head, atoi(citizenID));
    // for logfiles I check if this monitor process has the countryTo registered
    // in order to increment the respective counters
    struct CountryList* countryTo_temp = CountryList_Search(country_ptr, countryTo);

    if (skip_temp != NULL) {
        //printf("Success1:%d\n", success);
        success = check6Months(date, skip_temp->date_ptr->day, skip_temp->date_ptr->month, skip_temp->date_ptr->year);
        //printf("Success2:%d\n", success);
        if (success == -2) {
            printf("[check6Months] something went wrong\n");
            return -2;
        }

        if (success == -1) {
            //vaccination date is after travel date
            if (countryTo_temp != NULL)
                rejected++;

            return 1;
        }

        if (success == 1) {
            // citizen needs to be vaccinated again
            // printf("REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE
            // TRAVEL DATE\n");
            if (countryTo_temp != NULL)
                rejected++;

            return 2;
        }

        if (success == 0) {
            // happy travels
            // printf("REQUEST ACCEPTED – HAPPY TRAVELS\n");
            if (countryTo_temp != NULL)
                accepted++;

            return 0;
        }
    }
    return 0;
}

char * App_SearchVaccRec(char *citizenID) {

    return VaccineStatusCitizen(citizenID);
}

void App_AddVaccinationRec(struct Monitor_Assist* M, char* country) {

}

void App_SetCountryList(char* pathString) {
    //printf("App_SetCountryList Child:%d | %s\n",getpid(),pathString);

    numCountries = 0;
    char *path;
    path = strtok(pathString, "-");
    while (1) {
        //printf("%s\n", path);

        numCountries++;
        CountryList_Insert(&country_ptr, path);
        if ((path = strtok(NULL, "-")) == NULL)
            break;
    }
}

void App_SetInputDir(struct Monitor_Assist* M) {
    int lenght = strcspn(country_ptr->path, "/");
    M->inputDir = (char*) malloc(sizeof (char)* (lenght + 1));
    strncpy(M->inputDir, country_ptr->path, lenght);
    M->inputDir[lenght] = '\0';
}

void App_LogFile(){
    char log_file[64];
    sprintf(log_file,"log_file.%d",getpid());
    FILE* fp = fopen(log_file,"w+");

    if (fp == NULL) {
        perror("fopen Coordinator_LogFile");
    }

    struct CountryList *country_temp = country_ptr;

    while(country_temp != NULL){
        fprintf(fp,"%s\n",country_temp->country);

        country_temp = country_temp->next;
    }

     fprintf(fp, "TOTAL TRAVEL REQUESTS %d\nACCEPTED %d\nREJECTED %d\n", accepted + rejected, accepted, rejected);

     fclose(fp);
}