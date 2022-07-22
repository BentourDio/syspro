/* 
    Dionysios Mpentour 1115201300115

    Main function of the vaccineMonitor programm. The arguments are checked and the
    sequence for the data structure and IPC are set here.
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

#include "app.h"
#include "functions.h"
#include "monitor.h"
//./vaccineMonitor -c citizenRecordsFile –b bloomSize

int main(int argc, char *argv[]) {
    //FILE *fp;
    int bufferSize = 0;
    int bloomSize = 0;
    char * p_read;
    char * p_write;
    char * inputdir;
    struct Monitor_Assist M;

    srand(time(NULL));

    if (argc != 5) {
        printf("argc:%d\n",argc);
        printf("[Error] Wrong arguments Given for child %d \n", getpid());
        //printf("0:%s 1:%s 2:%s 3:%s 4%s\n",argv[0],argv[1],argv[2],argv[3],argv[4]);
        
        return -1;
    }

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-p1") == 0) {
            if (access(argv[i + 1], F_OK) != 0) {
                printf("Pipe doesn't exitst\nExiting\n");
                exit(EXIT_FAILURE);
            }

            p_read = argv[i + 1];
        }

        if (strcmp(argv[i], "-p2") == 0) {
            if (access(argv[i + 1], F_OK) != 0) {
                printf("Pipe doesn't exitst\nExiting\n");
                exit(EXIT_FAILURE);
            }
            p_write = argv[i + 1];
        }
    }

    Monitor_Assist_Init(&M, p_read, p_write, bufferSize, bloomSize);

    Monitor_OpenPipes(&M);

    read(M.fd_read, &bloomSize, sizeof (bloomSize));
    read(M.fd_read, &bufferSize, sizeof (bufferSize));
    read_data(M.fd_read, &inputdir, bufferSize);

    MonitorSet_InputDir(&M,inputdir);
    M.bloomSize = bloomSize;
    M.bufferSize = bufferSize;

    printf("Child %d [%s,%s,%s,%d,%d] \n", getpid(), inputdir, M.p_write, M.p_read, bloomSize, bufferSize);

    Monitor_Receive_Countries(&M);

    Monitor_SetHashTable(inputdir); //also creates hashtable
    Monitor_Load(inputdir, M);
    printf("After MonitorLoad\n");
    App_SetHelper(M);

    //printf("Child %d: Loading data is complete\n", getpid());

    Monitor_Send_Viruses(M);

    //    Monitor_Print();


    static struct sigaction act;

    act.sa_handler = App_catchinterrupt;
    sigfillset(&(act.sa_mask));

    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGUSR1, &act,NULL);
    
    Monitor_Application(M,inputdir);


    Monitor_Assist_Purge(M); // destroy hashtable too

    printf("EXITING MAIN OF CHILD: %d \n", getpid());

    return 0;
}