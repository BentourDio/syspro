Dionysios Mpentour 1115201300115 Project 3

TravelMonitorClient and monitorServer

The names might be misleading since I have reversed the roles of the two programms. The father
aka travelMonitorClient acts as the server since he creates the server and has the listening socket
and the child processes aka monitorServer act as the client. This implementation has been asked to the piazza
forum and has been accepted as a correct one. Using this implementation the functionality of the prj2 is transferred
directly to prj3. The names have been kept this way to adhere to the prj3's specifications.

-Compilication:
To compile these two programs simply type make. Each .o file is compiled individually and a common makefile is used
for the two programs. The programm is compiled with the -Wall flag with 2 warnings which should not be taken into 
consideration since that part of the code has been checked extensively and exists to the instructor's notes.

-Execution:
The program executes with arguments as specified from the instructors. If the number of countries' directories exceeds The
number of monitors then that is set to the number of dirs as to avoid inactive monitor processes. The names of the directories
are stored to an ordered double linked list. Then a code is assigned to each countrydir node which will be used to
share the directories to the monitors in roundrobin fashion.
Afterwards the serves is set up. I chose to create a master file descriptor and to share the same socket between the monitors
as to save the systems' resources and avoid the likely problem that a port is not avaible by a monitor process so it can't bind
connect.
Then a simple linked list is created where the fds are stored.