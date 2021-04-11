//Poojan Patel
//Justin Meng

#include "commando.h"
#include <stdio.h>
#include <string.h>
// cmdcol.c: functions related to cmdcol_t collections of commands.

void cmdcol_add(cmdcol_t *col, cmd_t *cmd){
    if(col->size < MAX_CMDS){               //verify adding will not exceed max # of cmds supported
        col->cmd[col->size] = cmd;          //add cmd to cmd array
        col->size++;			    //iterate size
    }
    else{
        printf("error: adding will exceed MAX_CMDS, the meaximum number of commands supported.");
    }					    //print on error
    return;
}
// Add the given cmd to the col structure. Update the cmd[] array and
// size field. Report an error if adding would cause size to exceed
// MAX_CMDS, the maximum number commands supported.

void cmdcol_print(cmdcol_t *col){
    printf("JOB  #PID      STAT   STR_STAT OUTB COMMAND\n");
    for(int i = 0; i<col->size; i++){
        printf("%-4d ", i);                                    //JOB
        printf("#%-8d ", (col->cmd[i])->pid);                  //PID
        printf("%4d ", (col->cmd[i])->status);                 //STAT
        printf("%10s ", (col->cmd[i])->str_status);            //STR_STAT
        printf("%4d ", (col->cmd[i])->output_size);            //OUTB
        for(int j = 0; (col->cmd[i])->argv[j] != NULL ; j++){
            printf("%s ", (col->cmd[i])->argv[j]);             //Commands
        }
        printf("\n");
    }   						       //loop
    return;
}
// Print all cmd elements in the given col structure.  The format of
// the table is
//
// JOB  #PID      STAT   STR_STAT OUTB COMMAND
// 0    #17434       0    EXIT(0) 2239 ls -l -a -F 
// 1    #17435       0    EXIT(0) 3936 gcc --help 
// 2    #17436      -1        RUN   -1 sleep 2 
// 3    #17437       0    EXIT(0)  921 cat Makefile 
// 
// Columns correspond to fields in the following way:
// JOB:      index in the cmdcol_t struct
// PID:      pid from the cmd_t struct
// STAT:     status from the cmd_t struct
// STR_STAT: str_status field from cmd_t
// OUTB:     output_size from the cmd_t struct
// COMMAND:  The contents of cmd->argv[] with a space
//           between each element of the array.
// 
// Widths of the fields and justification are as follows
// 
// JOB  #PID      STAT   STR_STAT OUTB COMMAND
// 1234 #12345678 1234 1234567890 1234 Remaining
// left  left    right      right rigt left
// int   int       int     string  int string

void cmdcol_update_state(cmdcol_t *col, int block){
    for(int i = 0; i<col->size; i++){
        cmd_update_state(col->cmd[i], block);		//run cmd_update_state for each cmd struct in cmdcol
    }
    return;
}
// Update each cmd in col by calling cmd_update_state() which is also
// passed the block argument (either NOBLOCK or DOBLOCK) 

void cmdcol_freeall(cmdcol_t *col){
    for(int i = 0; i<col->size; i++){
        cmd_free(col->cmd[i]);                          //run cmd_free for each cmd struct in col to free the cmd
    }
    return;
}
// Call cmd_free() on all of the constituent cmd_t's.
