CC = gcc
LIBFLAG = -g -std=gnu11 -Werror -Wall -Wextra -Wpedantic -Wmissing-declarations -Wmissing-prototypes -Wold-style-definition -o

all : mexec.c
	$(CC) $(LIBFLAG) mexec mexec.c

uninstall : 
	rm mexec