Dionysios Mpentour 1115201300115 Project 2

Bash-Script
Bash script has been implemented fully.
Using cut command and sort -u all the countries are registered and stored
into a temporary array. Then the directories for each country are created
containing their respective files for the records. Using grep and each country
at a time I collect all the entries for the respective country and store them into an array. Afterwards
the entries are entered in each file in a RoundRobin manner.

TravelMonitor & VaccineMonitor
-Compilication:

To compile these two programms simply type make. Each executive is compiled
individually and a common makefile is used for the two programms. The programm
is compiled with the -Wall flag with 2 warnings which should not be taken into 
consideration since that part of the code has been checked extensively.

The 2 programms consist of 30 individual files. The main function of each programm
is in the travelMonitor.c (which acts as the coordinator/father) and in the vaccineMonitor.c
(which acts as the children/monitor processes).

-Execution:

The programm executes with arguments as specified from the instructors. If the number of monitors
exceeds the number of the countries' directories, the sum of monitors is set to the number
of the subdirectories, as to avoid inactive children processes. The coordinator opens the inpute directory,
checks the subdirectories and stores their names in an ordered double linked list, which will be used
to assign pipes to countries and country directories to monitor processes in alphabetical RoundRobin
fashion.
Afterwards the coordinator creates the names of the to be named pipes in a simply linked link. Afterwards it checks
whether a file with the pipename exists, if it exists it unlinks and then executes mkfifo.
From the simply linked pipelist the pipes are assigned to countries which will be used for
the IPC of the coordinator and the monitors. 2 pipes (one for read and one for write) are used
to execute the IPC. The even number of nodes in the pipelist act as write (input) and the odd for
read (output). 2 pipes in a row are assigned to a monitor process.
The next step if fork and execl which calls the vaccineMonitor programm with arguments the 2 pipenames.
the vaccineMonitor is practically the PRJ-1 which has been extented for the purposes of the PRJ-2. That means
that a HashTable containing each individual citizenRecord data exists and skiplists and bloomfilters.
Because the number of files is high, at the start of each .c file a short summary is written which
explains the purpose of that file. Also highly detailed and sufficient comments exist especially in 
critical sections of the programm.

-IPC protocol
The communication protocol acts as specified from the instructors. Buffersize is used and a likewise
writeall and readall functions are implemented in the function.c file. For starters the size of the 
string to be written is sent and then, if needed string is sent into a single or multiple packets 
depending on the Buffersize. The read_data function reads the length of packet and then depending on the
Buffersize reads one packet or multiple. It will stop when there are no longer bytes to read. There is a 
warning from the -Wall flag for an uninitialized value, but the code there has been tested and works as intented.
Maybe the -Wall flag is a little outdated, because initialization is completed inside the if.

-Multiplexing:
Multiplexing is implemented fully with the use of the poll function.

-Signal handling.
Signals have been implemented fully with the use of sigaction to handle the respective SIGINT, SIGQUIT, SIGUSR1
and SIGCHLD signals, either by the coordinator or the monitors.  Also sigprocmask has been implemented to block
the signals when a critical section of code is running either on the father or the children.

All the requirements of the exercise have been implemented fully with the exception of the respawn. When a kill
command on a monitor process or if the child fails it sent a SIGCHLD to the father process. The father handles
the signals and does the appropriate actions with the pipes and pids, but when it goes on to create another child
with fork and exec the child produces a segmentation fault. So that part of the code has been commented out.

Some extra output has been maintained in order to demonstrate the correct execution of the programm.

Typing "print" in the API will print out all the countries that have been registered, which monitor (pid)
they have been assigned to, which pipes and which file descriptors have access to those pipes.

All data structure is freed at the end of execution.
The programm has been checked for memory leaks using valgrind.
A backup is maintained in my private respository of the university's linux servers.


Thank you for reading. I am looking forward to the examination.
