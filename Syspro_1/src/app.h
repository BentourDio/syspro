#ifndef APP
#define APP

#define MAXLVL 10

typedef struct CountryList *c;

void Load_File(FILE *fp);

void Load_CountryList(char *country, int age);

void Application(); // API

void TestSkipList();

void TestCountryList();

void Create_Htable(FILE *fp);

void Purge();

void Print();

// Application command functions*******************************

void VaccineStatus(char *citizenID, char *virusName); // done

void VaccineStatusCitizen(char *citizenID); // done

void nonVaccinatedPersons(char *virusName); // done

void VaccinateNow(char *citizenID, char *firstName, char *lastName,
                  char *country, int age, char *VirusName); // done

void InsertCitizenRecord(char *citizenID, char *firstName, char *lastName,
                         char *country, int age, char *virusName, char *vstatus,
                         int day, int month, int year); // done

void popStatusCountryDates(char *country, char *virusName, int d1, int m1,
                           int y1, int d2, int m2, int y2); // done

void popStatusVirus(char *virusName); // done

void PrintStatusVirus(struct CountryList *c_temp); // only for popStatusVirs & popstatusVirusDates

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
#endif