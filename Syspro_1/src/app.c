#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "app.h"
#include "countrylist.h"
#include "date_record.h"
#include "functions.h"
#include "myhashtable.h"
#include "mylist.h"
#include "myskiplist.h"
//#define  _POSIX_C_SOURCE 200809L
// 666 John Papadopoulos Greece 52 COVID-19 YES 27-12-2020
// 666 John Papadopoulos Greece 52 Η1Ν1 ΝΟ

static struct Hashtable idTable;
static struct Disease_List *dis_ptr = NULL;
static struct CountryList *country_ptr = NULL;

void Create_Htable(FILE *fp) {

  int hsize = set_tableSize(fp);

  idTable = Hashtable_Creation(hsize);
}

// void InsertPatientRecord( char *citizenID, char *name, char *surname, char
// *country, char *virusName, int age, int vstatus, int iday, int imonth, int
// iyear) {
//
//}

void Load_CountryList(char *country, int age) {
  struct CountryList *temp = NULL;

  temp = CountryList_Search(country_ptr, country);

  if (temp == NULL) {
    temp = CountryList_Insert(&country_ptr, country);
  } else {
    groupIncrement(&temp, age);
  }
}

void Load_File(FILE *fp) {

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
    // printf("%s",line);
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
        printf("[ERROR] Entry with id: %s is YES but no date is given\n",citizenID);
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
        printf("[ERROR] Entry with id: %s is NO but date is given\n",citizenID);
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
    struct Hash_Entry *htemp = Hashtable_SearchID(
        citizenID, idTable.htable[Hash_key(citizenID, idTable.size)]);

    if (htemp == NULL) {
      int f = Htable_Insert(idTable, citizenID, name, surname, country,
                            virusName, atoi(age));

      if (f == -1) {
        printf("### register failed for id: %s  (error) ", citizenID);
        exit(1);
      }
      /*
      at this point an entry has surely been made, so i update country simply
      linked list which holds information about the population and the age
      groups of that population
      The country list is updated only when a new citizen registers
       */
      // Load_CountryList(country, atoi(age));

      htemp = Hashtable_SearchID(
          citizenID, idTable.htable[Hash_key(
                         citizenID, idTable.size)]); // pointer to position of
    } else {
      if (crossData(htemp, citizenID, name, surname, country, atoi(age)) ==
          -1) {
        printf("invalid data for citizen\n");
        printf("[Warning] citizenID for id: %s (already exists)\n",
               htemp->citizenID);
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

    if (dtemp == NULL) {
      Disease_List_Insert(&dis_ptr, virusName, htemp);
      dtemp = Disease_List_Search(dis_ptr, virusName);
    }

    if (vstatus == 1) {
      dtemp->unvacc_head =
          Skip_List_Insert(dtemp->unvacc_head, atoi(citizenID), iday, imonth,
                           iyear, vstatus, htemp);
    }
    if (vstatus == 0) {
      dtemp->vacc_head = Skip_List_Insert(dtemp->vacc_head, atoi(citizenID),
                                          iday, imonth, iyear, vstatus, htemp);
    }
  }

  if (line) {
    free(line);
  }
}

void Application() // diepafi tou xristi me command line
{
  char buffer[2048];
  char buffer2[2048];

  while (1) {
    printf("-->");

    if (fgets(buffer2, sizeof(buffer), stdin) == NULL) {
      break;
    }

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
      Print();
    }
    if (strcmp(buffer, "/exit") == 0) {
      break;
    }

    if (strncmp(buffer, "/vaccineStatusBloom", 19) == 0) {
      // TODO
      char *thr, *citizenID, *virusname;
      thr = strtok(buffer, " \t");
      citizenID = strtok(NULL, " \t");
      virusname = strtok(NULL, " \t");

      if (thr != NULL && citizenID != NULL && virusname != NULL) {
        // TODO
      }
    }

    if (strncmp(buffer, "/vaccineStatus", 14) == 0) {
      char *thr, *citizenID, *virusname;

      thr = strtok(buffer, " \t");
      citizenID = strtok(NULL, " \t");
      virusname = strtok(NULL, " \t");

      if (thr != NULL && citizenID != NULL &&
          virusname != NULL) /// vaccineStatus citizenID virusName
      {
        VaccineStatus(citizenID, virusname);
      }
      if (thr != NULL && citizenID != NULL &&
          virusname == NULL) /// vaccineStatus citizenID
      {
        VaccineStatusCitizen(citizenID);
      }
    }
    /// populationStatus [country] virusName date1 date2
    if (strncmp(buffer, "/populationStatus", 17) == 0) {
      char *thr, *p1, *p2, *p3, *p4;

      thr = strtok(buffer, " \t");
      p1 = strtok(NULL, " \t");
      p2 = strtok(NULL, " \t");
      p3 = strtok(NULL, " \t");
      p4 = strtok(NULL, " \t");

      if (thr != NULL && p1 != NULL && p2 != NULL && p3 == NULL && p4 == NULL) {
        PopulationStatusCountry(p1, p2);
      }
      if (thr != NULL && p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL) {
        int d1, m1, y1, d2, m2, y2;
        d1 = atoi(strtok(p3, "-"));
        m1 = atoi(strtok(NULL, "-"));
        y1 = atoi(strtok(NULL, " "));
        d2 = atoi(strtok(p4, "-"));
        m2 = atoi(strtok(NULL, "-"));
        y2 = atoi(strtok(NULL, " "));

        PopulationStatusCountryDates(p1, p2, d1, m1, y1, d2, m2, y2);
      }
      if (thr != NULL && p1 != NULL && p2 == NULL && p3 == NULL && p4 == NULL) {
        PopulationStatus(p1);
      }
      if (thr != NULL && p1 != NULL && p2 != NULL && p3 != NULL && p4 == NULL) {

        int d1, m1, y1, d2, m2, y2;
        d1 = atoi(strtok(p2, "-"));
        m1 = atoi(strtok(NULL, "-"));
        y1 = atoi(strtok(NULL, " "));
        d2 = atoi(strtok(p3, "-"));
        m2 = atoi(strtok(NULL, "-"));
        y2 = atoi(strtok(NULL, " "));

        PopulationStatusDates(p1, d1, m1, y1, d2, m2, y2);
      }
    }

    /// popStatusByAge [country] virusName date1 date2
    if (strncmp(buffer, "/popStatusByAge", 15) == 0) {
      char *thr, *p1, *p2, *p3, *p4;

      thr = strtok(buffer, " \t");
      p1 = strtok(NULL, " \t"); // country
      p2 = strtok(NULL, " \t"); // virusname
      p3 = strtok(NULL, " \t"); // date1
      p4 = strtok(NULL, " \t"); // date2

      if (thr != NULL && p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL) {
        // p1 = counrty, p2 = virusname, p3 = date1, p4 =date2
        // popStatusByaAge country virusName date1 date2
        int d1, m1, y1, d2, m2, y2;
        d1 = atoi(strtok(p3, "-"));
        m1 = atoi(strtok(NULL, "-"));
        y1 = atoi(strtok(NULL, " "));
        d2 = atoi(strtok(p4, "-"));
        m2 = atoi(strtok(NULL, "-"));
        y2 = atoi(strtok(NULL, " "));

        popStatusCountryDates(p1, p2, d1, m1, y1, d2, m2, y2);
      }
      if (thr != NULL && p1 != NULL && p2 != NULL && p3 != NULL && p4 == NULL) {
        // p1 = virusname, p2 = date1, p3 = date2
        // popStatusByAge virusName date1 date2
        int d1, m1, y1, d2, m2, y2;
        d1 = atoi(strtok(p2, "-"));
        m1 = atoi(strtok(NULL, "-"));
        y1 = atoi(strtok(NULL, " "));
        d2 = atoi(strtok(p3, "-"));
        m2 = atoi(strtok(NULL, "-"));
        y2 = atoi(strtok(NULL, " "));

        popStatusVirusDates(p1, d1, m1, y1, d2, m2, y2);
      }
      if (thr != NULL && p1 != NULL && p2 != NULL && p3 == NULL && p4 == NULL) {
        // popStatusByAge country virusName
        popStatusCountry(p1, p2);
      }
      if (thr != NULL && p1 != NULL && p2 == NULL && p3 == NULL && p4 == NULL) {
        // popStatusByAge virusName
        popStatusVirus(p1);
      }
    }

    /// insertCitizenRecord citizenID firstName lastName country age virusName
    /// YES/NO [date]
    if (strncmp(buffer, "/insertCitizenRecord", 20) == 0) {
      char *thr, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8;

      thr = strtok(buffer, " \t");
      p1 = strtok(NULL, " \t"); // citizenID
      p2 = strtok(NULL, " \t"); // firstName
      p3 = strtok(NULL, " \t"); // lastName
      p4 = strtok(NULL, " \t"); // country
      p5 = strtok(NULL, " \t"); // age
      p6 = strtok(NULL, " \t"); // virusName
      p7 = strtok(NULL, " \t"); // YES|NO
      p8 = strtok(NULL, " \t"); // date

      if (thr != NULL && p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL &&
          p5 != NULL && p6 != NULL && p7 != NULL && p8 != NULL) {

        if (strcmp(p7, "NO") == 0) { // since date!=NULL invalid entry
          printf("invalid date. Vstatus = %s & date = %s\n", p7, p8);
          continue;
        }
        int day, month, year;
        day = atoi(strtok(p8, "-"));
        month = atoi(strtok(NULL, "-"));
        year = atoi(strtok(NULL, " "));

        InsertCitizenRecord(p1, p2, p3, p4, atoi(p5), p6, p7, day, month, year);
      }
      if (thr != NULL && p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL &&
          p5 != NULL && p6 != NULL && p7 != NULL && p8 == NULL) {

        if (strcmp(p7, "YES") == 0) {
          printf("invalid date. Vstatus =%s & date = NULL\n", p7);
          continue;
        }

        InsertCitizenRecord(p1, p2, p3, p4, atoi(p5), p6, p7, 0, 0, 0);
      }
    }

    /// vaccinateNow citizenID firstName lastName country age virusName
    if (strncmp(buffer, "/vaccinateNow", 13) == 0) {
      char *thr, *p1, *p2, *p3, *p4, *p5, *p6;

      thr = strtok(buffer, " \t");
      p1 = strtok(NULL, " \t");
      p2 = strtok(NULL, " \t");
      p3 = strtok(NULL, " \t");
      p4 = strtok(NULL, " \t");
      p5 = strtok(NULL, " \t");
      p6 = strtok(NULL, " \t");

      if (thr != NULL && p1 != NULL && p2 != NULL && p3 != NULL && p4 != NULL &&
          p5 != NULL && p6 != NULL) {
        VaccinateNow(p1, p2, p3, p4, atoi(p5), p6);
      }
    }

    /// list-nonVaccinated-Persons virusName
    if (strncmp(buffer, "/list-nonVaccinated-Persons", 27) == 0) {
      char *thr, *virusName;

      thr = strtok(buffer, " \t");
      virusName = strtok(NULL, " \t");

      if (thr != NULL && virusName != NULL) {
        nonVaccinatedPersons(virusName);
      }
    }

  } // end of while(1)
}

void VaccineStatus(char *citizenID, char *virusName) {
  // check if citizen exists in database
  struct Hash_Entry *hash_temp = Hashtable_SearchID(
      citizenID, idTable.htable[Hash_key(citizenID, idTable.size)]);

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
    printf("VACCINATED ON %d-%d-%d\n", skip_temp->date_ptr->day,
           skip_temp->date_ptr->month, skip_temp->date_ptr->year);
  } else {
    printf("NOT VACCINATED\n");
  }
}

void VaccineStatusCitizen(char *citizenID) {
  // check if citizenID exists
  struct Hash_Entry *hash_temp = Hashtable_SearchID(
      citizenID, idTable.htable[Hash_key(citizenID, idTable.size)]);

  if (hash_temp == NULL) {
    printf("[Citizen not registered in database]\n");
    return;
  }
  // find the virus
  struct Disease_List *d_temp = dis_ptr;
  // transend skip lists and if citizenID found print data
  while (d_temp != NULL) {
    struct Skip_List_Node *skip_temp;
    // transcend vaccinated skip list
    skip_temp = Skip_List_Search(d_temp->vacc_head, atoi(citizenID));

    if (skip_temp != NULL) {
      if (skip_temp->date_ptr->vstatus == 0) {
        printf("%s YES %d-%d-%d\n", d_temp->virusName, skip_temp->date_ptr->day,
               skip_temp->date_ptr->month, skip_temp->date_ptr->year);
      }
    }
    // search non vaccinated skip list for citizenID
    skip_temp = Skip_List_Search(d_temp->unvacc_head, atoi(citizenID));

    if (skip_temp != NULL) {
      if (skip_temp->date_ptr->vstatus == 1) {
        printf("%s NO\n", d_temp->virusName);
      }
    }
    d_temp = d_temp->next;
  }
}

void nonVaccinatedPersons(char *virusName) {
  struct Disease_List *d_temp = Disease_List_Search(dis_ptr, virusName);

  if (d_temp == NULL) {
    printf("[Virus:%s does not exist in database]\n", virusName);
    return;
  }
  // print for virusName all it's non vacc list records
  Skip_List_Print_Level(d_temp->unvacc_head);
}

void VaccinateNow(char *citizenID, char *firstName, char *lastName,
                  char *country, int age, char *virusName) {
  // check if citizen exists in database

  struct Hash_Entry *hash_temp = Hashtable_SearchID(
      citizenID, idTable.htable[Hash_key(citizenID, idTable.size)]);
  // if citizen doesn't exist insert in idTable first and find its position
  if (hash_temp == NULL) {

    printf("[Citizen does not exist in database]\n");

    int f = Htable_Insert(idTable, citizenID, firstName, lastName, country,
                          virusName, age);
    if (f == -1) { // error checking for hashtable insert
      printf("### register failed for id: %s  (error) ", citizenID);
      exit(1);
    }
    /*
      at this point an entry has surely been made, so i update country simply
      linked list which holds information about the population and the age
      groups of that population.
      The country list is updated only when a new citizen registers
     */
    // Load_CountryList(country, age);

    hash_temp = Hashtable_SearchID(
        citizenID, idTable.htable[Hash_key(citizenID, idTable.size)]);

  } else {
    if (crossData(hash_temp, citizenID, firstName, lastName, country, age) ==
        -1) {
      printf("Citizen exists in database but data do not match");
      return;
    }
  }
  // generate today's date
  int day, month, year;
  GenerateTodayDate(&day, &month, &year);
  // find the virus node

  struct Disease_List *d_temp = Disease_List_Search(dis_ptr, virusName);
  // if the virus doesn't exist in database insert it in the list
  if (d_temp == NULL) {
    printf("[Virus does not exist in database]\n");

    Disease_List_Insert(&dis_ptr, virusName, hash_temp);
    // get the location of the newly inserted virus node
    d_temp = Disease_List_Search(dis_ptr, virusName);
    // insert citizen in newly created vacc skip list
    d_temp->vacc_head = Skip_List_Insert(d_temp->vacc_head, atoi(citizenID),
                                         day, month, year, 0, hash_temp);
    return; // my job here is done
            // the citizen has been inserted and since he is new entry
            // he can't exist in non vacc skip list
  }
  // since d_temp!=NULL check if citizen exists in vacc|non vacc list
  struct Skip_List_Node *skip_temp;
  skip_temp = Skip_List_Search(d_temp->vacc_head, atoi(citizenID));
  if (skip_temp != NULL) {
    if (skip_temp->date_ptr->vstatus == 0) {
      printf("ERROR : CITIZEN: %d ALREADY VACCINATED ON %0d-%0d-%d\n",
             skip_temp->citizenID, skip_temp->date_ptr->day,
             skip_temp->date_ptr->month, skip_temp->date_ptr->year);
    }
    /*this works because in order for citizen to exist in vaccinated list
    he has to exist in the hashtable and the virus must exist in disease list
    so no malloc has been done, which means i can safely return
     */
    return;
  } else {
    // insert in vaccinated skip list
    d_temp->vacc_head = Skip_List_Insert(d_temp->vacc_head, atoi(citizenID),
                                         day, month, year, 0, hash_temp);
  }
  // now check the non vacc list
  skip_temp = Skip_List_Search(d_temp->unvacc_head, atoi(citizenID));
  if (skip_temp != NULL) {
    // citizen is in non vacc list which means i have to remove him from there
    // he already is in vaccinated skip list
    Skip_List_Delete(&(d_temp->unvacc_head), atoi(citizenID));
  }
}

void InsertCitizenRecord(char *citizenID, char *firstName, char *lastName,
                         char *country, int age, char *virusName, char *vstatus,
                         int day, int month, int year) {
  // check if citizen exists in hashtable
  struct Hash_Entry *hash_temp = Hashtable_SearchID(
      citizenID, idTable.htable[Hash_key(citizenID, idTable.size)]);

  if (hash_temp == NULL) {
    // since he doesn't exist insert in idTable
    printf("[Citizen does not exist in database]\n");

    int f = Htable_Insert(idTable, citizenID, firstName, lastName, country,
                          virusName, age);
    if (f == -1) { // error checking for hashtable insert
      printf("### register failed for id: %s  (error) ", citizenID);
      exit(1);
    }
    // update|insert in countrylist
    // Load_CountryList(country, age);
    // get the position of entry in idTable
    hash_temp = Hashtable_SearchID(
        citizenID, idTable.htable[Hash_key(citizenID, idTable.size)]);
  } else { // since register exists check if it has correct data
    if (crossData(hash_temp, citizenID, firstName, lastName, country, age) ==
        -1) {
      printf("Citizen exists in database but data do not match");
      return;
    }
  }
  // check if the virus exist in disease list
  struct Disease_List *d_temp = Disease_List_Search(dis_ptr, virusName);
  // if the virus doesn't exist insert it
  if (d_temp == NULL) {
    printf("[Virus does not exist in database]\n");

    Disease_List_Insert(&dis_ptr, virusName, hash_temp);
    // get location of newly inserted node
    d_temp = Disease_List_Search(dis_ptr, virusName);

    if (strcmp(vstatus, "YES") == 0) {
      d_temp->vacc_head = Skip_List_Insert(d_temp->vacc_head, atoi(citizenID),
                                           day, month, year, 0, hash_temp);
    }
    if (strcmp(vstatus, "NO") == 0) {
      d_temp->unvacc_head = Skip_List_Insert(
          d_temp->unvacc_head, atoi(citizenID), day, month, year, 1, hash_temp);
    }
    return; // my job here is done likewise with vaccinateNow
  }         // end of if(d_temp==NULL)
  /*at this point the virus exists in disease list and if a record exists in
  the vacc or non vacc skip lists, that means that no insert in hashtable has
  been made so i can safely error check and if error return.
   */
  /*
  1st if YES check the vacc list. If found on vacc list --> ERROR
  if not found on vacc list insert in vacc list and check non vacc list
  if found on non vacc list remove him from non vacc list
   */
  struct Skip_List_Node *skip_temp;

  if (strcmp(vstatus, "YES") == 0) {
    // search the vacc skip list to see if record exists
    skip_temp = Skip_List_Search(d_temp->vacc_head, atoi(citizenID));

    if (skip_temp != NULL) {
      printf("ERROR CITIZEN: %d ALREADY VACCINATED ON %0d-%0d-%d\n",
             skip_temp->citizenID, skip_temp->date_ptr->day,
             skip_temp->date_ptr->month, skip_temp->date_ptr->year);
      return;
    } else {
      d_temp->vacc_head = Skip_List_Insert(d_temp->vacc_head, atoi(citizenID),
                                           day, month, year, 0, hash_temp);
    }
    // check non vacc list to remove if needed
    skip_temp = Skip_List_Search(d_temp->unvacc_head, atoi(citizenID));
    if (skip_temp != NULL) {
      Skip_List_Delete(&(d_temp->unvacc_head), atoi(citizenID));
    }
  }
  /*
  2nd if NO check the vacc list. If found on vacc list --> ERROR
  you can't unvaccinate a vaccinated person you dumbass punk.
  Then check the non vacc list. If found --> ERROR
  else insert in non vacc list
   */
  if (strcmp(vstatus, "NO") == 0) {
    skip_temp = Skip_List_Search(d_temp->vacc_head, atoi(citizenID));

    if (skip_temp != NULL) {
      printf("ERROR CITIZEN: %d ALREADY VACCINATED ON %0d-%0d-%d\n",
             skip_temp->citizenID, skip_temp->date_ptr->day,
             skip_temp->date_ptr->month, skip_temp->date_ptr->year);
      return;
    }
    // then check the non vacc list
    skip_temp = Skip_List_Search(d_temp->unvacc_head, atoi(citizenID));
    if (skip_temp != NULL) {
      printf("ERROR CITIZEN: %d ALREADY ΝΟΤ VACCINATED \n",
             skip_temp->citizenID);
      return;
    } else {
      d_temp->unvacc_head = Skip_List_Insert(
          d_temp->unvacc_head, atoi(citizenID), day, month, year, 1, hash_temp);
    }
  }
}
//*******************************************************************************
// popStatusByAge Functions
void popStatusCountryDates(char *country, char *virusName, int d1, int m1,
                           int y1, int d2, int m2, int y2) {
  // check if dates check out first
  if (datecmp(d1, m1, y1, d2, m2, y2) >= 0) {
    printf("Date1 > Date2\n");
    return;
  }
  // check if virus exists in database first
  struct Disease_List *d_temp = Disease_List_Search(dis_ptr, virusName);

  if (d_temp == NULL) {
    printf("[ERROR] Virus doesn't exist in database or no second date was "
           "given \n");
    return;
  }

  struct Skip_List_Node *skip_temp = d_temp->vacc_head->level[0];
  struct CountryList *country_node = NULL;
  // first check vacc skip list for citizens
  while (skip_temp != NULL) {
    if (sandwichDates(d1, m1, y1, d2, m2, y2, skip_temp) == 0) {
      if (strcmp(skip_temp->info_ptr->country, country) == 0) {
        // sandwichdates checks if entry date is sandwiched between d1& d2

        if (country_node == NULL) {
          CountryList_Init(&country_node, country);
          groupIncrement(&country_node, skip_temp->info_ptr->age);
        } else {
          groupIncrement(&country_node, skip_temp->info_ptr->age);
        }
      }
    } else {
      if (strcmp(skip_temp->info_ptr->country, country) == 0) {
        if (country_node == NULL) {
          CountryList_Init(&country_node, country);
          overallIncrement(&country_node, skip_temp->info_ptr->age);
        } else {
          overallIncrement(&country_node, skip_temp->info_ptr->age);
        }
      }
    }
    skip_temp = skip_temp->level[0];
  }

  // then check the non vacc skip list for citizens of this country
  skip_temp = d_temp->unvacc_head->level[0];

  while (skip_temp != NULL) {
    if (strcmp(skip_temp->info_ptr->country, country) == 0) {
      if (country_node == NULL) {
        CountryList_Init(&country_node, country);
        nonvaccIncrement(&country_node, skip_temp->info_ptr->age);
      } else {
        nonvaccIncrement(&country_node, skip_temp->info_ptr->age);
      }
    }
    skip_temp = skip_temp->level[0];
  }

  // Print Statistics
  if (country_node == NULL) {
    printf("[ERROR] Country: %s doesn't exist in database\n", country);
    return;
  }
  PrintStatusVirus(country_node);
  // end of function destroy temporary structures
  CountryList_Purge(country_node);
}

void popStatusCountry(char *country, char *virusName) {
  // first check if virus exists in database
  struct Disease_List *d_temp = Disease_List_Search(dis_ptr, virusName);

  if (d_temp == NULL) {
    printf("[ERROR] Virus doesn't exist in database or no second date was "
           "given\n");
    return;
  }

  struct Skip_List_Node *skip_temp = d_temp->vacc_head->level[0];
  struct CountryList *country_node = NULL;

  // transcend vaccinated persons skip list
  while (skip_temp != NULL) {
    if (strcmp(skip_temp->info_ptr->country, country) == 0) {

      if (country_node == NULL) {
        CountryList_Init(&country_node, country);
        groupIncrement(&country_node, skip_temp->info_ptr->age);
      } else {
        groupIncrement(&country_node, skip_temp->info_ptr->age);
      }
    }
    skip_temp = skip_temp->level[0];
  }

  // then transcend the non vaccinated persons list
  skip_temp = d_temp->unvacc_head->level[0];

  while (skip_temp != NULL) {
    if (strcmp(skip_temp->info_ptr->country, country) == 0) {

      if (country_node == NULL) {
        CountryList_Init(&country_node, country);
        nonvaccIncrement(&country_node, skip_temp->info_ptr->age);
      } else {
        nonvaccIncrement(&country_node, skip_temp->info_ptr->age);
      }
    }
    skip_temp = skip_temp->level[0];
  }
  // Print Statistics
  if (country_node == NULL) {
    printf("Country : %s doesn't exist in database\n", country);
    return;
  }
  PrintStatusVirus(country_node);
  // destroy temporary structures
  CountryList_Purge(country_node);
}

void popStatusVirus(char *virusName) {

  // first check if virus exists in database
  struct Disease_List *d_temp = Disease_List_Search(dis_ptr, virusName);

  if (d_temp == NULL) {
    printf("[ERROR] Virus doesn't exist in database or no second date was "
           "given\n");
    return;
  }

  struct Skip_List_Node *skip_temp = d_temp->vacc_head->level[0];
  struct CountryList *country_head = NULL;
  // check first the vaccinated skip list of virus
  while (skip_temp != NULL) {

    struct CountryList *c_temp =
        CountryList_Search(country_head, skip_temp->info_ptr->country);

    if (c_temp == NULL) {
      c_temp = CountryList_Insert(&country_head, skip_temp->info_ptr->country);
      groupIncrement(&c_temp, skip_temp->info_ptr->age);

    } else {
      groupIncrement(&c_temp, skip_temp->info_ptr->age);
    }
    skip_temp = skip_temp->level[0];
  }

  // then transcend the non vaccinated persons list
  skip_temp = d_temp->unvacc_head->level[0];

  while (skip_temp != NULL) {

    struct CountryList *c_temp =
        CountryList_Search(country_head, skip_temp->info_ptr->country);
    if (c_temp == NULL) {

      c_temp = CountryList_Insert(&country_head, skip_temp->info_ptr->country);
      nonvaccIncrement(&c_temp, skip_temp->info_ptr->age);
    } else {
      nonvaccIncrement(&c_temp, skip_temp->info_ptr->age);
    }
    skip_temp = skip_temp->level[0];
  }

  // Print Statistics
  struct CountryList *c_temp = country_head;
  while (c_temp != NULL) {

    PrintStatusVirus(c_temp);

    c_temp = c_temp->next;
  }

  // Delete temporary structures
  CountryList_Purge(country_head);
}

void popStatusVirusDates(char *virusName, int d1, int m1, int y1, int d2,
                         int m2, int y2) {
  // check if dates check out first
  if (datecmp(d1, m1, y1, d2, m2, y2) >= 0) {
    printf("Date1 > Date2\n");
    return;
  }
  // check if virus exists in database first
  struct Disease_List *d_temp = Disease_List_Search(dis_ptr, virusName);

  if (d_temp == NULL) {
    printf("[ERROR] Virus doesn't exist in database or no second date was "
           "given\n");
    return;
  }

  struct Skip_List_Node *skip_temp = d_temp->vacc_head->level[0];
  struct CountryList *country_head = NULL;
  // first check vacc skip list for citizens
  while (skip_temp != NULL) {
    if (sandwichDates(d1, m1, y1, d2, m2, y2, skip_temp) == 0) {
      // sandwichdates checks if entry date is sandwiched between d1& d2
      struct CountryList *c_temp =
          CountryList_Search(country_head, skip_temp->info_ptr->country);

      if (c_temp == NULL) {
        c_temp =
            CountryList_Insert(&country_head, skip_temp->info_ptr->country);

        groupIncrement(&c_temp, skip_temp->info_ptr->age);

      } else {
        groupIncrement(&c_temp, skip_temp->info_ptr->age);
      }
    } else {
      struct CountryList *c_temp =
          CountryList_Search(country_head, skip_temp->info_ptr->country);
      if (c_temp == NULL) {
        c_temp =
            CountryList_Insert(&country_head, skip_temp->info_ptr->country);

        overallIncrement(&c_temp, skip_temp->info_ptr->age);
      } else {
        overallIncrement(&c_temp, skip_temp->info_ptr->age);
      }
    }
    skip_temp = skip_temp->level[0];
  }
  // then check the non vacc skip list for citizen of this country
  skip_temp = d_temp->unvacc_head->level[0];

  while (skip_temp != NULL) {

    struct CountryList *c_temp =
        CountryList_Search(country_head, skip_temp->info_ptr->country);
    if (c_temp == NULL) {
      c_temp = CountryList_Insert(&country_head, skip_temp->info_ptr->country);
      nonvaccIncrement(&c_temp, skip_temp->info_ptr->age);
    } else {
      nonvaccIncrement(&c_temp, skip_temp->info_ptr->age);
    }
    skip_temp = skip_temp->level[0];
  }

  // Print Statistics
  struct CountryList *c_temp = country_head;
  while (c_temp != NULL) {

    PrintStatusVirus(c_temp);

    c_temp = c_temp->next;
  }
  // end of function
  CountryList_Purge(country_head);
}

void PrintStatusVirus(struct CountryList *c_temp) {
  // only for popStatusVirus & popStatusVirusDates
  printf("%s\n", c_temp->country);
  if (c_temp->agegroup1_unvacc == 0 && c_temp->agegroup1 == 0) {
    printf("0-20: 0 0\n");
  } else {
    printf(
        "0-20: %d %2.2f%%\n", c_temp->agegroup1,
        (float)100 * c_temp->agegroup1 /
            (c_temp->agegroup1 + c_temp->agegroup1_unvacc + c_temp->overall1));
  }
  if (c_temp->agegroup2_unvacc == 0 && c_temp->agegroup2 == 0) {
    printf("20-40: 0 0%%\n");
  } else {
    printf(
        "20-40: %d %2.2f%%\n", c_temp->agegroup2,
        (float)100 * c_temp->agegroup2 /
            (c_temp->agegroup2 + c_temp->agegroup2_unvacc + c_temp->overall2));
  }
  if (c_temp->agegroup3_unvacc == 0 && c_temp->agegroup3 == 0) {
    printf("40-60: 0 0%%\n");
  } else {
    printf(
        "40-60: %d %2.2f%%\n", c_temp->agegroup3,
        (float)100 * c_temp->agegroup3 /
            (c_temp->agegroup3 + c_temp->agegroup3_unvacc + c_temp->overall3));
  }
  if (c_temp->agegroup4_unvacc == 0 && c_temp->agegroup4 == 0) {
    printf("60+  : 0 0%%\n");
  } else {
    printf(
        "60+  : %d %2.2f%%\n", c_temp->agegroup4,
        (float)100 * c_temp->agegroup4 /
            (c_temp->agegroup4 + c_temp->agegroup4_unvacc + c_temp->overall4));
  }
}
//*******************************************************************************
// popStatusByAge Functions END

void Purge() {
  Hashtable_Purge(idTable);

  Disease_List_Purge(dis_ptr);

  // CountryList_Purge(country_ptr);
}
// *******************************************************************************************
// Population Status
void PopulationStatus(char *virusName) {
  struct CountryList *country_list = NULL;

  struct Disease_List *d_temp = Disease_List_Search(dis_ptr, virusName);

  if (d_temp == NULL) {
    printf("the virus: %s doesn't exist in database\n", virusName);
    return;
  }

  struct Skip_List_Node *skip_temp = d_temp->vacc_head->level[0];
  // first check the vaccinated skip list for citizens
  while (skip_temp != NULL) {
    struct CountryList *c_temp =
        CountryList_Search(country_list, skip_temp->info_ptr->country);

    if (c_temp == NULL) {
      c_temp = CountryList_Insert(&country_list, skip_temp->info_ptr->country);
      c_temp->vacc_summ++;

    } else {
      c_temp->vacc_summ++;
    }
    skip_temp = skip_temp->level[0];
  }
  // then check the unvaccinated skip list for citizens
  skip_temp = d_temp->unvacc_head->level[0];
  while (skip_temp != NULL) {
    struct CountryList *c_temp =
        CountryList_Search(country_list, skip_temp->info_ptr->country);

    if (c_temp == NULL) {
      c_temp = CountryList_Insert(&country_list, skip_temp->info_ptr->country);
      c_temp->unvacc_summ++;
    } else {
      c_temp->unvacc_summ++;
    }
    skip_temp = skip_temp->level[0];
  }
  // Print Statistics
  struct CountryList *c_temp = country_list;
  while (c_temp != NULL) {
    printf("%s %d %2.2f%% vacc: %d unvacc:%d \n", c_temp->country,
           c_temp->vacc_summ,
           (float)100 * c_temp->vacc_summ /
               (c_temp->vacc_summ + c_temp->unvacc_summ),
           c_temp->vacc_summ, c_temp->unvacc_summ);
    c_temp = c_temp->next;
  }
  CountryList_Purge(country_list);
}

void PopulationStatusDates(char *virusName, int d1, int m1, int y1, int d2,
                           int m2, int y2) {
  // chech if dates are legit first
  if (datecmp(d1, m1, y1, d2, m2, y2) >= 0) {
    printf("Date1 > Date2\n");
    return;
  }
  // check if virus exists in database
  struct Disease_List *d_temp = Disease_List_Search(dis_ptr, virusName);

  if (d_temp == NULL) {
    printf("[ERROR] Virus doesn't exist in database or no second date was "
           "given\n");
    return;
  }

  struct Skip_List_Node *skip_temp = d_temp->vacc_head->level[0];
  struct CountryList *country_head = NULL;
  // transcend vaccinated skip list
  while (skip_temp != NULL) {
    if (sandwichDates(d1, m1, y1, d2, m2, y2, skip_temp) == 0) {
      // sandwichdates checks if entry date is sandwiched between d1& d2
      struct CountryList *c_temp =
          CountryList_Search(country_head, skip_temp->info_ptr->country);

      if (c_temp == NULL) {
        // country doesn't exist in temporary list so insert it
        c_temp =
            CountryList_Insert(&country_head, skip_temp->info_ptr->country);
        c_temp->vacc_summ++;
        c_temp->vacc_summ_overall++;
      } else {
        c_temp->vacc_summ++;
        c_temp->vacc_summ_overall++;
      }
    } else {
      struct CountryList *c_temp =
          CountryList_Search(country_head, skip_temp->info_ptr->country);
      if (c_temp == NULL) {
        // country doesn't exist in temporary list so insert it
        c_temp =
            CountryList_Insert(&country_head, skip_temp->info_ptr->country);
        c_temp->vacc_summ_overall++;
      } else {
        c_temp->vacc_summ_overall++;
      }
    }
    skip_temp = skip_temp->level[0];
  }

  // now transcend unvaccinated skip list
  skip_temp = d_temp->unvacc_head->level[0];
  while (skip_temp != NULL) {
    // sandwichdates checks if entry date is sandwiched between d1& d2
    // EDIT can't check dates for unvacc
    struct CountryList *c_temp =
        CountryList_Search(country_head, skip_temp->info_ptr->country);

    if (c_temp == NULL) {
      // country doesn't exist in temporary list so insert it
      c_temp = CountryList_Insert(&country_head, skip_temp->info_ptr->country);
      c_temp->unvacc_summ++;
    } else {
      c_temp->unvacc_summ++;
    }

    skip_temp = skip_temp->level[0];
  }
  // print Statistics
  struct CountryList *c_temp = country_head;
  while (c_temp != NULL) {
    printf("%s %d %2.2f%% vacc: %d unvacc:%d \n", c_temp->country,
           c_temp->vacc_summ,
           (float)100 * c_temp->vacc_summ /
               (c_temp->vacc_summ_overall + c_temp->unvacc_summ),
           c_temp->vacc_summ, c_temp->unvacc_summ);
    c_temp = c_temp->next;
  }
  // purge temporary structures
  CountryList_Purge(country_head);
}

void PopulationStatusCountry(char *country, char *virusName) {

  // check for virus in database
  struct Disease_List *d_temp = Disease_List_Search(dis_ptr, virusName);

  if (d_temp == NULL) {
    printf("the virus: %s doesn't exist in database\n", virusName);
    return;
  }

  struct Skip_List_Node *skip_temp = d_temp->vacc_head->level[0];
  struct CountryList *country_node = NULL;
  // transcend vaccinated persons skip list
  while (skip_temp != NULL) {

    if (strcmp(skip_temp->info_ptr->country, country) == 0) {

      if (country_node == NULL) {

        CountryList_Init(&country_node, country);
        country_node->vacc_summ++;
      } else {
        country_node->vacc_summ++;
      }
    }
    skip_temp = skip_temp->level[0];
  }
  // transcend non vaccinated skip list
  skip_temp = d_temp->unvacc_head->level[0];

  while (skip_temp != NULL) {
    if (strcmp(skip_temp->info_ptr->country, country) == 0) {

      if (country_node == NULL) {
        CountryList_Init(&country_node, country);
        country_node->unvacc_summ++;
      } else {
        country_node->unvacc_summ++;
      }
    }
    skip_temp = skip_temp->level[0];
  }

  // Print Statistics
  if (country_node == NULL) {
    printf("Country : %s NOT FOUND\n", country);
    return;
  }

  printf("%s %d %2.2f%% unvacc: %d\n", country_node->country,
         country_node->vacc_summ,
         (float)100 * country_node->vacc_summ /
             (country_node->vacc_summ + country_node->unvacc_summ),
         country_node->unvacc_summ);
  // Purge temporary structures
  CountryList_Purge(country_node);
}

void PopulationStatusCountryDates(char *country, char *virusName, int d1,
                                  int m1, int y1, int d2, int m2, int y2) {
  // check dates legitimacy
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

  struct Skip_List_Node *skip_temp = d_temp->vacc_head->level[0];
  struct CountryList *country_node = NULL;

  // transcend vaccinated skip list
  while (skip_temp != NULL) {
    if (sandwichDates(d1, m1, y1, d2, m2, y2, skip_temp) == 0) {

      if (strcmp(skip_temp->info_ptr->country, country) == 0) {

        if (country_node == NULL) {
          CountryList_Init(&country_node, country);
          country_node->vacc_summ++;
          country_node->vacc_summ_overall++;

        } else if (country_node != NULL) {
          country_node->vacc_summ++;
          country_node->vacc_summ_overall++;
        }
      }
    } else {
      if (strcmp(skip_temp->info_ptr->country, country) == 0) {
        if (country_node == NULL) {

          CountryList_Init(&country_node, country);
          country_node->vacc_summ_overall++;

        } else if (country_node != NULL) {
          country_node->vacc_summ_overall++;
        }
      }
    }
    skip_temp = skip_temp->level[0];
  }

  // now transcend unvaccinated skip list
  skip_temp = d_temp->unvacc_head->level[0];

  while (skip_temp != NULL) {
    if (strcmp(skip_temp->info_ptr->country, country) == 0) {
      if (country_node == NULL) {
        CountryList_Init(&country_node, country);
        country_node->unvacc_summ++;
      } else if (country_node != NULL) {
        country_node->unvacc_summ++;
      }
    }
    skip_temp = skip_temp->level[0];
  }
  if (country_node == NULL) {
    printf("Country : %s NOT FOUND\n", country);
    return;
  }
  // Print Statistics
  printf("%s %d %f%% unvacc: %d\n", country_node->country,
         country_node->vacc_summ,
         (float)100 * country_node->vacc_summ /
             (country_node->vacc_summ_overall + country_node->unvacc_summ),
         country_node->unvacc_summ);

  // purge temporary structures
  CountryList_Purge(country_node);
}
//********************************************************************************************
void Print() {
  Hashtable_Print(idTable);

  Disease_List_Print(dis_ptr);
}

void TestSkipList() {
  printf("starting test\n");
  // set_tableSize(fp, idTable);
  struct Skip_List_Node *h;
  struct Hash_Entry *t = NULL;
  h = Skip_List_Node_Init(-1, MAXLVL, -1, -1, -1);

  for (int i = 0; i < 100; i++) {
    h = Skip_List_Insert(h, rand() % 100000, 0, 0, 0, 0, t);
    printf("%d \n", i);
    h = Skip_List_Insert(h, rand() % 100000, 0, 0, 0, 0, t);
  }
  // h = Skip_List_Insert(h, 5, 0, 0, 0, 0, t);
  // h = Skip_List_Insert(h, 5, 0, 0, 0, 0, t);
  h = Skip_List_Insert(h, 5, 0, 0, 0, 0, t);
  h = Skip_List_Insert(h, 43, 0, 0, 0, 0, t);
  h = Skip_List_Insert(h, 12, 0, 0, 0, 0, t);
  h = Skip_List_Insert(h, 567, 0, 0, 0, 0, t);
  h = Skip_List_Insert(h, 23, 0, 0, 0, 0, t);
  h = Skip_List_Insert(h, 666, 0, 0, 0, 0, t);

  Print_Skip_List(h);

  Skip_List_Delete(&h, 666);
  Skip_List_Delete(&h, 666);
  Skip_List_Delete(&h, 5);
  Skip_List_Delete(&h, 12);
  Skip_List_Delete(&h, 567);
  Skip_List_Delete(&h, 23);
  Skip_List_Delete(&h, 43);

  Print_Skip_List(h);
  Test_Sequence_Skip_List(h);

  Skip_List_Purge(h);

  exit(0);
}
