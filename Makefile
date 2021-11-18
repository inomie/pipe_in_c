CC = gcc
LIBFLAG = -g -std=gnu11 -Wall

all : mexec 

mexec : mexec_func.o mexec.o
	$(CC) $(LIBFLAG) -o mexec mexec.o mexec_func.o

mexec.o : mexec.c mexec_func.h
	$(CC) $(LIBFLAG) -c -o mexec.o mexec.c

mexec_func.o : mexec_func.c mexec_func.h
	$(CC) $(LIBFLAG) -c -o mexec_func.o mexec_func.c

uninstall : 
	rm mexec

clean : 
	rm mexec.o mexec_func.o