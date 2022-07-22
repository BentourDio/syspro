# intellectual property of Dionysios Mpentour All Rights Reserved

OBJS 	= app.o mylist.o functions.o main.o myhashtable.o countrylist.o date_record.o mybloomfilter.o myskiplist.o hash_functions.o
SOURCE	= app.c mylist.c functions.c main.c myhashtable.c countrylist.c date_record.c mybloomfilter.c myskiplist.c hash_functions.c
HEADER  = app.h mylist.h functions.h myhashtable.h countrylist.h date_record.h mybloomfilter.h myskiplist.h hash_functions.h
OUT  	= vaccineMonitor
CC	= gcc
FLAGS   = -g -ggdb -c -Wall

$(OUT): $(OBJS)
	$(CC) -g $(OBJS) -o $@

# create/compile the individual files >>separately<< 
main.o: main.c
	$(CC) $(FLAGS) main.c

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
	valgrind  --leak-check=full --show-leak-kinds=all -s ./vaccineMonitor -c records.txt -b 15

valgrind2:
	valgrind  --leak-check=full --show-leak-kinds=all ./vaccineMonitor -c test.txt -b 15

valgrind3:
	valgrind  --leak-check=full --show-leak-kinds=all ./vaccineMonitor -c medium.txt -b 15

valgrind4:
	valgrind  --leak-check=full --show-leak-kinds=all ./vaccineMonitor -c large.txt -b 15

valgrind5:
	valgrind  --leak-check=full --show-leak-kinds=all ./vaccineMonitor -c no.txt -b 15

run1:
	./vaccineMonitor -c record.txt -b 15

run3:
	./vaccineMonitor -c no.txt -b 15

run2:
	./vaccineMonitor -c test.txt -b 15

clean:
	rm -f $(OBJS) $(OUT)
