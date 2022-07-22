#ifndef APP
#define APP

#include "coordinator.h"
#define MAXLVL 10

#include "monitor.h"

typedef struct CountryList *c;



void Load_File(FILE *fp, int bloomsize, pthread_mutex_t * lock);

void Load_CountryList(char *country, int age);

void Application(int bloomSize); // API

void TestSkipList();

void TestCountryList();

void Create_Htable(int hsize);

void Purge();

void Print();

// Application command functions*******************************

void StatusBloom(char *citizenID, char *virusName);

void VaccineStatus(char *citizenID, char *virusName); // done

char* VaccineStatusCitizen(char *citizenID); // done

void InsertCitizenRecord(char *citizenID, char *firstName, char *lastName,
                         char *country, int age, char *virusName, char *vstatus,
                         int day, int month, int year, int bloomSize); // done



void Bloom();

//int main_vaccine_monitor(int argc, char *argv[]);

void App_catchinterrupt(int signo);

void Import_Countries(struct Monitor_Assist *M);

void SetHashTable();

int App_LoadDir(struct Monitor_Assist *M);

void SendViruses(struct Monitor_Assist *M);

int App_TravelRequest(char* citizenID, char* date, char* countryFrom, char* countryTo,char* virusName);

char * App_SearchVaccRec(char* citizenID);

void App_WriteLogFile();

void App_AddVaccinationRec(struct Monitor_Assist* M, char* country);

void App_SetHelper(struct Monitor_Assist M);

void App_SetCountryList(char* );

void App_SetInputDir(struct Monitor_Assist* M);

void addVaccinationRecords(char* country);

int App_LoadSingleDir(struct Monitor_Assist, char* country);

void App_LogFile();
#endif