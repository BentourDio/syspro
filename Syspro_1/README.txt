Dionysios Mpentour 1115201300115

The program called vaccineMonitor consists of 19 files which are:
app.c/.h	This file acts like a monitor for the program. It opens the records’ file and appropriately processes the strings as to load the information to the data structures. It also holds the command prompt for the API of the vaccineMonitor with the user and the functions that are required of the project.
countrylist.c/.h	This file contains an abstract data structure which is used as a temporary list in some functions.
date_record.c/.h	This file holds an abstract data structure with information for the vaccination date of a record and is pointed to from a skip list entry.
functions.c/.h	This file consists of useful functions that are called many times in the code, so I created this file in order to make the code more readable.
hash_functions.c/.h	This file consists of the hashing functions that have been provided to us for the bloomfilter data structure.
main.c		Holds the main function of the vaccineMonitor.
mybloomfilter.c/.h  This file contains the bloomfilter data structure and it’s relevant functions. The bloomfilter structure it’s functions have not been implemented.
myhashtable.c/.h	This file contains a hashtable data type structure which is used to store each entry’s information (name, surname, country, age) as to avoid any duplication.
mylist.c/.h	This file contains a simply linked list data type structure which is used to store the viruses and have pointers to their other data structures (vaccinated and non vaccinated skip lists, bloomfilter, hashtable entry).
myskiplist.c/.h		This file contains the skip list data type structure and it’s relevant functions.

A makefile is also included in which separate compilation is being used. In order to compile just type make and in order to remove the executables just type make clean.

All the required data structures and functions of the project have been implemented with the exception of the bloomfilter and it’s relevant function which is the /vaccineStatusBloom.

Consistent Data Structures:
As it has been mentioned a simply linked list with the viruses’ names is created (virusName is the key) which have 2 skip list pointers for their respective vaccinated and non vaccinated persons skip list. I chose a simply linked skip list because the number of viruses is relatively small so access to it can be considered O(1).
The skip list data structure consists of nodes which have as key the citizenID converted to integer. It’s data consist of a pointer to the citizen’s information (hashtable type pointer) and a pointer to the date information. It also has an array skip list type pointers which emulate the levels of the skip list. Each array has MAXLVL pointers but if those are been put to use depends on how many levels the individual skip list node appears, which is set in a random manner. MAXLVL is set to 10 via define in the app.h header file. It has been commented on piazza that we can make an assumption in order to avoid an unfair coin toss which increase the skip level indefinitely. 10 is the the log(2)GlobalPopulation / 3 (not all humans will have a record for every vaccine). The head of the skip list is a simple skip list node with no data which points to the rest of the skip list. The end of the skip list is NULL.
The last consistent data structure is the hashtable. Each entry of a skip list points to the hashtable as to access their information. A record can appear in multiple viruses’ skip lists but all those nodes point to the same hashtable node as to avoid data duplication. Also the hashtable is the best way to check if a patient has already been registered in the database and to check their information if they are to be registered for multiple vaccines.

Execution:
By executing vaccineMonitor the main function is invoked and the first task it does is to check the arguments and if the record file exists. Then the hashtable is created whose size is the number of the records file divided by 2. Afterwards the Load_File function is invoked which using the getline and strtok manages the strings and the entries and loads them appropriately to the data structures. Error checking also takes place with the relevant stdout happening. VaccineMonitor continues with the Application() functions which acts as the API. All the functions work as intended and according to the piazza reccomendations with the exception of the /vaccineBloomStatus which has not been implemented.
The statistics of the /populationStatus and the /popStatusByAge are implemented according to the pinned post on piazza. In order to store those statistics for each country and age group a simply linked list with the country as key is created. I chose a simply linked list because in order to print all the statistics all nodes will have to be accessed so the complexity is O(n) regardless of the data structure. The simply linked list (or node in the case for a specific country are chosen) are freed at the end of the functions.
Please refer to the comments in the source files as they are very detailed to the complicated and sensitive parts of these functions. The README is restricted to two pages.
By exiting the Application function the deletion of the data structures will be invoked leaving no memory leaks or errors.
The program has been tested with valgrind both to my personal computer and the uni’s workstations. The compilation is done without warnings and errors.

Bash Script:
Bash Script has been implemented fully. Giving the parameter duplicatesAllowed=0 duplicates are allowed, meanwhile duplicatesAllowed=1 duplicates are not allowed and the maximum of possible unique entries is limited to 10k.
The bash script is divided to functions which generate randomly each part of the data.
In the case of duplicates not allowed, inconsistent entries can be generated (like an entry having a YES and no date or a NO with a date).
The bash script checks the number of arguments and sets them on variables at the start. Then it opens the two files storing their contents to two arrays from which viruses and countries are chosen randomly in their respective functions. If duplicates are allowed then the relative function is invoked creating possible duplicates for an entry for multiple vaccines. The entries can be consistent or inconsistent. If duplicates are not allowed, a patient can't have multiple entries for different vaccines and the entry can be both consistent and inconsistent.
The script has been tested in the uni’s workstations successfully.
A sample countries.txt and a viruses.txt have been included for fast testing.
Please forgive my humour for some functions names or prints. The program was really long testing my mental capabilities. Thank you.
