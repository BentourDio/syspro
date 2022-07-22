#ifndef COORDINATOR_H
#define COORDINATOR_H

typedef struct CountryDirList* countrydirlist;

int Load_Dir(char *inputDir);

void Coordinator_RoundRob(int monitor_sum);

void Coordinator_catchinterrupt(int signo);

void Coordinator_Purge();

void Coordinator_Print();

void Coordinator_Load_Sockets(int numMon);

void CoordinatorApplication(int bufferSize, int bloomSize);

void Coordinator_Create_Master_Socket(int port);

void Coordinator_Fork_Children(char *inputDir, int _numMon, int socketSize, int bloomSize,int numDir, int cyclicSize, int numThreads, int port);

void Coordinator_Send_Countries(int bufferSize);

void Coordinator_Receive_Viruses(int bufferSize, int bloomSize);

void Coordinator_Receive_Viruses_Polling(int bufferSize, int bloomSize, int numMon);

void Coordinator_Shutdown_Children(int numMon);

void Coordinator_Assign_Sockets();

void Coordinator_Destroy_Sockets();

void Test();

//-----------------------------------------------------------------------------

void TravelRequest(char* citizenID, char* date, char* countryFrom, char* countryTo, char* virusName, int bufferSize);

void Send_TravelRequest(struct CountryDirList* dis_temp,char *citizenID, char *date, char *countryFrom, char *countryTo, char *virusName, int bufferSize,int* success);

void TravelStats(char* virusName, char* date1, char* date2);

void TravelStatsCountry(char *virusName, char *date1, char *date2, char* country);

void AddVaccinationRecords(char *country,int bufferSize,int bloomSize);

void SearchVaccinationStatus(char* citizenID, int bufferSize, int n);

void SearchVaccinationStatusPolling(char* citizenID, int bufferSize, int n);

void Load_TravelStatsList(char *message, int code);

void Coordinator_LogFile();

void Coordinator_ReceiveVirusFromChild(struct CountryDirList*,char*,int bufferize, int bloomSize);

void OperationLazarusComeOutakaRespawn(int deadpid);

void SetCoordinator_Assist(char *inputDir, int bloomSize, int bufferSize);

void Send_Exit(int socketSize);
#endif