# Quasi Command Line Shell: Commando

## General Overview
A program for a quasi-command line shell in C called Commando which is designed to have functionalities similar to bash (default shell in many Linux machines), but also to have properties which distinguish it. 

# Usage

>bash% make

>>gcc -Wall -g -c commando.c

>>gcc -Wall -g -c cmd.c

>>gcc -Wall -g -c cmdcol.c

>>gcc -Wall -g -c util.c

>>gcc -Wall -g -o commando commando.o cmd.o cmdcol.o util.o

>bash% ./commando

>@> help

>>COMMANDO COMMANDS

>>help               : show this message

>>exit               : exit the program

>>list               : list all jobs that have been started giving information on each

>>pause nanos secs   : pause for the given number of nanseconds and seconds

>>output-for int     : print the output for given job number

>>output-all         : print output for all jobs

>>wait-for int       : wait until the given job number finishes

>>wait-all           : wait for all jobs to finish

>>command arg1 ...   : non-built-in is run as a job            # Runs a command as a child process

>@> list

>>JOB  #PID      STAT   STR_STAT OUTB COMMAND

etc.

## blah

## blah

