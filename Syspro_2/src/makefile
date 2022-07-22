# intellectual property of Dionysios Mpentour All Rights Reserved

OBJS 	= app.o mylist.o functions.o myhashtable.o countrylist.o date_record.o mybloomfilter.o myskiplist.o hash_functions.o coordinator.o monitor.o countrydir_list.o pipelist.o travelReq_list.o
SOURCE	= app.c mylist.c functions.c travelMonitor.c myhashtable.c countrylist.c date_record.c mybloomfilter.c myskiplist.c hash_functions.c coordinator.c monitor.c countrydir_list.c pipelist.c travelReq_list.c
HEADER  = app.h mylist.h functions.h myhashtable.h countrylist.h date_record.h mybloomfilter.h myskiplist.h hash_functions.h coordinator.h monitor.h countrydir_list.h pipelist.h travelReq_list.h
OUT1  	= travelMonitor
OUT2	= vaccineMonitor
CC	= gcc
FLAGS   = -g -ggdb -c -Wall
FLAGS2	= -g -ggdb -Wall

all:$(OUT1) $(OUT2)


$(OUT1): travelMonitor.o $(OBJS)
	$(CC) $(FLAGS2) travelMonitor.o $(OBJS) -o $(OUT1)


$(OUT2): vaccineMonitor.o $(OBJS)
	$(CC) $(FLAGS2) vaccineMonitor.o $(OBJS) -o $(OUT2)

# create/compile the individual files >>separately<< 
travelMonitor.o: travelMonitor.c
	$(CC) $(FLAGS) travelMonitor.c

travelReq_list.o: travelReq_list.c
	$(CC) $(FLAGS) travelReq_list.c

vaccineMonitor.o: vaccineMonitor.c
	$(CC) $(FLAGS) vaccineMonitor.c

pipelist.o: pipelist.c
	$(CC) $(FLAGS) pipelist.c

countrydir_list.o: countrydir_list.c
	$(CC) $(FLAGS) countrydir_list.c

monitor.o: monitor.c
	$(CC) $(FLAGS) monitor.c

coordinator.o: coordinator.c
	$(CC) $(FLAGS) coordinator.c

hash_functions.o: hash_functions.c
	$(CC) $(FLAGS) hash_functions.c

myskiplist: myskiplist.c
	$(CC) $(FLAGS) myskiplist.c
app.o: app.c
	$(CC) $(FLAGS) app.c

mybloomfilter: mybloomfilter.c
	$(CC) $(FLAGS) mybloomfilter.c

date_record.o: date_record.c
	$(CC) $(FLAGS) date_record.c

mylist.o: mylist.c
	$(CC) $(FLAGS) mylist.c

countrylist.o: countrylist.c
	$(CC) $(FLAGS) countrylist.c

myhashtable.o: myhashtable.c
	$(CC) $(FLAGS) myhashtable.c

functions.o: functions.c
	$(CC) $(FLAGS) functions.c

valgrind:
	valgrind  --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./travelMonitor -m 3 -b 512 -s 256 -i data_small


valgrind2:
	valgrind  --leak-check=full --show-leak-kinds=all -s --trace-children=yes ./travelMonitor -m 3 -b 512 -s 256 -i data_medium


run:
	./travelMonitor -m 1 -b 512 -s 256 -i data_small
run1:
	./travelMonitor -m 4 -b 512 -s 256 -i data_small

run2:
	./travelMonitor -m 4 -b 512 -s 256 -i data_medium

run3:
	./travelMonitor -m 1 -b 512 -s 256 -i data_medium

clean:
	rm -f $(OBJS) $(OUT1) $(OUT2) travelMonitor.o vaccineMonitor.o

#//./travelMonitor –m numMonitors -b bufferSize -s sizeOfBloom -i input_dir