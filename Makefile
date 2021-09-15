CC = gcc
LIBFLAG = -g -std=gnu11 -Werror -Wall -Wextra -Wpedantic -Wmissing-declarations -Wmissing-prototypes -Wold-style-definition -o

all : mexec 
	gcc -g -std=gnu11 -Wall -o mexec mexec.o

mexec : mexec.c
	gcc -g -std=gnu11 -Wall   -c -o mexec.o mexec.c

uninstall : 
	rm mexec

clean : 
	rm mexec.o