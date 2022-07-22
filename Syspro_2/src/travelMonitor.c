/* 
    Dionysios Mpentour 1115201300115

    main function of the travelMonitor programm
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

#include "app.h"
#include "functions.h"
#include "coordinator.h"

//./travelMonitor –m numMonitors -b bufferSize -s sizeOfBloom -i input_dir

int main(int argc, char *argv[]) {
    
    int bloomSize, numMonitors, bufferSize;
    char inputDir[PATH_MAX];

    if (argc != 9) {
        printf("[Error] Wrong arguments Given\nExiting\n");
        return -1;
    }

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-m") == 0) {
            numMonitors = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-b") == 0) {
            bufferSize = atoi(argv[i + 1]);
        }
        if (strcmp(argv[i], "-i") == 0) {
            strcpy(inputDir, argv[i + 1]);
        }
        if (strcmp(argv[i], "-s") == 0) {
            bloomSize = atoi(argv[i + 1]);
        }
    }
    int numDir = Load_Dir(inputDir);
    //if the directories are more than the numMonitor equalise the 2 values
    if (numDir < numMonitors)
        numMonitors = numDir;

    SetCoordinator_Assist(inputDir, bloomSize, bufferSize);
    Coordinator_Load_Pipes(numMonitors);

    

    Coordinator_Create_Pipes();

    Coordinator_Assign_Pipes();

    Coordinator_Print();

    Coordinator_Fork_Children(inputDir, numMonitors, bufferSize, bloomSize);

    Coordinator_Send_Countries(bufferSize);

    Coordinator_Receive_Viruses_Polling(bufferSize, bloomSize, numMonitors);

    printf("Working on: %s\n", inputDir);

    static struct sigaction act;

    act.sa_handler = Coordinator_catchinterrupt;
    sigfillset(&(act.sa_mask));


    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    //sigaction(SIGCHLD,&act,NULL);

    CoordinatorApplication(bufferSize, bloomSize);
    //I set the act to default because when sigkill is sent to children
    // they will send sigchld to father causing the handler to respawn
    act.sa_handler = SIG_DFL;
    sigaction(SIGCHLD, &act, NULL);

    Coordinator_Shutdown_Children(numMonitors);

    Coordinator_Destroy_Pipes();

     // Coordinator_WriteLog();  // TODO


    Coordinator_Purge();

    printf("EXITING MAIN\n");

    return 0;
}