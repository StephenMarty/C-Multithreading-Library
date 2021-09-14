CC = gcc

all: lib_smthread.o lib_smthread.a driver1.o driver1 driver2.o driver2 driver3.o driver3 driver4.o driver4 driver5.o driver5

## LIBRARY CREATION ##
lib_smthread.o: lib_smthread.c
	$(CC) -c lib_smthread.c -o lib_smthread.o 

lib_smthread.a: lib_smthread.o
	ar rcs lib_smthread.a lib_smthread.o

## DRIVER PROGRAMS ##
driver1.o: driver1.c
	$(CC) -c driver1.c -o driver1.o

driver1: driver1.o
	$(CC) -o driver1 driver1.o -L. -l_smthread
	
driver2.o: driver2.c
	$(CC) -c driver2.c -o driver2.o

driver2: driver2.o
	$(CC) -o driver2 driver2.o -L. -l_smthread
	
driver3.o: driver3.c
	$(CC) -c driver3.c -o driver3.o

driver3: driver3.o
	$(CC) -o driver3 driver3.o -L. -l_smthread

driver4.o: driver4.c
	$(CC) -c driver4.c -o driver4.o

driver4: driver4.o
	$(CC) -o driver4 driver4.o -L. -l_smthread

driver5.o: driver5.c
	$(CC) -c driver5.c -o driver5.o

driver5: driver5.o
	$(CC) -o driver5 driver5.o -L. -l_smthread


## CLEAN INSTRUCTIONS ##
clean:
	rm -f *.o lib_smthread.a driver1 driver2 driver3 driver4 driver5 driver6 driver7

cleanexec:
	rm -f driver1 driver2 driver3 driver4 driver5 driver6 driver7

cleanlib:
	rm -f lib_smthread.a

cleanobj:
	rm -f *.o
