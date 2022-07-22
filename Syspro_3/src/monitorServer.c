/* 
    Dionysios Mpentour 1115201300115

    Main function of the monitorServer programm. The arguments are checked and the
    sequence for the data structures and the socket creation are set here.
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
#include <time.h>
#include <netinet/in.h>

#include "app.h"
#include "functions.h"
#include "monitor.h"
#include "ConsumersQueue.h"


//monitorServer -p port -t numThreads -b socketSize -c cyclicSize -s sizeOfBloom path1 path2 ... pathn

//min argc = 10 + 1

int main(int argc, char *argv[]) {

    /*for (int i =0; i <argc; i ++){
        printf("[[argv[%d]:%s]]\n",i,argv[i]);
    }*/
    int numThreads = 0, port, cyclicSize = 0;
    int bloomSize = 0;
    int socketSize = 0;
    struct Monitor_Assist M;

    srand(getpid());

    if (argc < 11) {
        printf("argc:%d\n", argc);
        printf("[Error] Not enough arguments Given for child %d \n", getpid());
        return -1;
    }

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            port = atoi(argv[i + 1]);
        }

        if (strcmp(argv[i], "-t") == 0) {
            numThreads = atoi(argv[i + 1]);
        }

        if (strcmp(argv[i], "-b") == 0) {
            socketSize = atoi(argv[i + 1]);
        }

        if (strcmp(argv[i], "-c") == 0) {
            cyclicSize = atoi(argv[i + 1]);
        }


        if (strcmp(argv[i], "-s") == 0) {
            bloomSize = atoi(argv[i + 1]);
        }
    }
    printf("port=%d numThread=%d socketS=%d cyclicS=%d bloomS=%d\n",port ,numThreads,socketSize,cyclicSize,bloomSize);
    Monitor_Assist_Init(&M, port, numThreads, socketSize, cyclicSize, bloomSize);

    //SetAssistAddVac(&M);


    Monitor_OpenSocket(&M);

    Monitor_CreateThreads(&M);
    
    //get the paths from the arguments

    //printf("Child:%d argc:%d with directories:%s\n", getpid(), argc, argv[11]);

    App_SetCountryList(argv[11]);

    App_SetInputDir(&M);
    
    Monitor_SetHashTable(); //also creates hashtable

    Monitor_Load(&M);

    
    printf("Child %d after monitor load: [%d,%d] \n", getpid(),bloomSize, socketSize);

    //printf("Child %d: Loading data is complete\n", getpid());
    
    //printf("@@$$ ABOUT TO ENTER SEND VIRUSES $$@@\n");
    Monitor_Send_Viruses(M);

    //Monitor_Print();

    //Print();
    
    
    Monitor_Application(&M);

    App_LogFile();
    
    Monitor_DestroyThreads(&M);

    Monitor_Assist_Purge(M); // destroy hashtable too
    

    printf("EXITING MAIN OF CHILD: %d \n", getpid());

    return 0;
}