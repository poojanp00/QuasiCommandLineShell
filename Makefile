

CFLAGS = -Wall -Werror -g
CC     = gcc $(CFLAGS)

commando: commando.o cmd.o cmdcol.o commando.h util.o
			$(CC) -o commando commando.o cmd.o cmdcol.o util.o

commando.o: commando.c commando.h 
			$(CC) -c commando.c

cmd.o: cmd.c commando.h
			$(CC) -c cmd.c

cmdcol.o: cmdcol.c commando.h
			$(CC) -c cmdcol.c

util.o: util.c commando.h
			$(CC) -c util.c




clean: 
			rm -f *.o commando
			rm -f *.o cmd
			rm -f *.o cmdcol
			rm -f *.o util


#include test_Makefile		# add at the END of Makefile

