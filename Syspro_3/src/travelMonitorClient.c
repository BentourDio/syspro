/* 
    Dionysios Mpentour 1115201300115

    main function of the travelMonitorClient programm
    arguments are checked and the sequence of the coordinator
    functions is called here
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <netinet/in.h>

#include "app.h"
#include "functions.h"
#include "coordinator.h"

//./travelMonitorClient –m numMonitors -b socketBufferSize -c cyclicBufferSize -s sizeOfBloom -i input_dir -t numThreads

int main(int argc, char *argv[]) {

    int bloomSize, numMonitors, socketSize, cyclicSize, numThreads;
    char inputDir[PATH_MAX];
    srand(getpid());
    int port = (rand()% (63000 -4000 + 1) + 4000);

    if (argc != 13) {
        printf("[Error] Wrong arguments Given {Coordinator}\nExiting\n");

        return -1;
    }

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-m") == 0) {
            numMonitors = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-b") == 0) {
            socketSize = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-i") == 0) {
            strcpy(inputDir, argv[i + 1]);
        }
        if (strcmp(argv[i], "-s") == 0) {
            bloomSize = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-t") == 0) {
            numThreads = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-c") == 0) {
            cyclicSize = atoi(argv[i + 1]);
        }
    }
    int numDir = Load_Dir(inputDir);
    //if the directories are more than the numMonitor equalise the 2 values
    if (numDir < numMonitors)
        numMonitors = numDir;

    Coordinator_RoundRob(numMonitors);

    Coordinator_Create_Master_Socket(port);
    
    Coordinator_Load_Sockets(numMonitors);

    Coordinator_Assign_Sockets();

    Coordinator_Print();

    static struct sigaction act;

    act.sa_handler = Coordinator_catchinterrupt;
    sigfillset(&(act.sa_mask));

    sigaction(SIGFPE, &act, NULL );

    Coordinator_Fork_Children(inputDir, numMonitors, socketSize, bloomSize, numDir, cyclicSize, numThreads, port);

    Coordinator_Receive_Viruses_Polling(socketSize, bloomSize, numMonitors);



//    Coordinator_Receive_Viruses(socketSize, bloomSize);

    CoordinatorApplication(socketSize, bloomSize);

    Coordinator_Shutdown_Children(numMonitors);

    Coordinator_Destroy_Sockets();

//    Coordinator_WriteLog(); // TODO

    Coordinator_LogFile();
    Coordinator_Purge();

    printf("EXITING MAIN\n");

    return 0;
}