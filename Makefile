CC = gcc
LIBFLAG = -g -std=gnu11 -Wall

all : mexec 

mexec : mexec.o
	$(CC) $(LIBFLAG) -o mexec mexec.o

mexec.o : mexec.c
	$(CC) $(LIBFLAG)   -c -o mexec.o mexec.c

uninstall : 
	rm mexec

clean : 
	rm mexec.o