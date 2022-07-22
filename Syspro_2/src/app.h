#ifndef APP
#define APP

#define MAXLVL 10

#include "monitor.h"

typedef struct CountryList *c;



void Load_File(FILE *fp, int bloomsize);

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

void nonVaccinatedPersons(char *virusName); // done

void VaccinateNow(char *citizenID, char *firstName, char *lastName,
                  char *country, int age, char *VirusName,
                  int bloomSize); // done

void InsertCitizenRecord(char *citizenID, char *firstName, char *lastName,
                         char *country, int age, char *virusName, char *vstatus,
                         int day, int month, int year, int bloomSize); // done

void popStatusCountryDates(char *country, char *virusName, int d1, int m1,
                           int y1, int d2, int m2, int y2); // done

void popStatusVirus(char *virusName); // done

void PrintStatusVirus( struct CountryList *c_temp); // only for popStatusVirs & popstatusVirusDates

void popStatusCountry(char *country, char *virusName); // done

void popStatusVirusDates(char *virusName, int d1, int m1, int y1, int d2,
                         int m2, int y2); // done

void PopulationStatus(char *virusName); // done

void PopulationStatusDates(char *virusName, int d1, int m1, int y1, int d2,
                           int m2, int y2); // done

void PopulationStatusCountry(char *country, char *virusName); // done

void PopulationStatusCountryDates(char *country, char *virusName, int d1,
                                  int m1, int y1, int d2, int m2,
                                  int y2); // done
// end of Application functions*****************************************

void Bloom();

//int main_vaccine_monitor(int argc, char *argv[]);

// ************************* 2nd assignment ***********************

void App_catchinterrupt(int signo);

void Import_Countries(struct Monitor_Assist *M);

void SetHashTable(char * inputdir);

void App_LoadDir(char *inputdir, struct Monitor_Assist M);

void SendViruses(struct Monitor_Assist M);

int App_TravelRequest(char* citizenID, char* date, char* countryFrom, char* countryTo,char* virusName);

char * App_SearchVaccRec(char* citizenID);

void App_WriteLogFile();

void App_AddVaccinationRec(char* country);

void App_SetHelper(struct Monitor_Assist M);
#endif