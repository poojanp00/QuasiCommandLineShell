//Poojan Patel
//Justin Meng

#include <stdio.h>
#include <string.h>
#include "commando.h"


int main(int argc, char *argv[]){
    setvbuf(stdout, NULL, _IONBF, 0);                 // Turn off output buffering
    char input[MAX_LINE];                             //initialize variables
    int ntok = 0;
    char *tokens[ARG_MAX];
    int echoing = 0;                                  //echo flag: 1 for on; 0 for off (starts off(
    cmdcol_t *col1 = malloc(sizeof(cmdcol_t));	      //malloc cmdcol structure
    col1->size = 0; 				      //initialize size to 0
    if(argc > 1){				      //check there is more than one element in argv
      if(strcmp(argv[1], "--echo") == 0){             //enabling echoing
              echoing = 1; 
          }
     }
    while(1){                                         //entering main loop
        printf("@> ");                                //print prompt for user
        char * ret = fgets(input, MAX_LINE, stdin);   //get input from stdin to the input tape
        if(ret == NULL){ 
            printf("\nEnd of input\n");
            break;                                    //break if end of input
        }
        else{
            if(echoing == 1 || getenv("COMMANDO_ECHO") != NULL){ //if echoing enabled, echo the input
                printf("%s", input);
            }
            parse_into_tokens(input, tokens, &ntok);             //parse input into readable tokens
	    if(tokens[0] == NULL){
		continue;                                        //check if user hit 'enter/return' key, if so keep asking for input
	    }
            else if(strcmp(tokens[0], "help") == 0){
                        printf("COMMANDO COMMANDS\n"
                        "help               : show this message\n"
                        "exit               : exit the program\n"
                        "list               : list all jobs that have been started giving information on each\n"
                        "pause nanos secs   : pause for the given number of nanseconds and seconds\n"
                        "output-for int     : print the output for given job number\n"
                        "output-all         : print output for all jobs\n"
                        "wait-for int       : wait until the given job number finishes\n"
                        "wait-all           : wait for all jobs to finish\n"
                        "command arg1 ...   : non-built-in is run as a job\n");
            }                                              //print help menu
	    else if(strcmp(tokens[0], "exit") == 0){

                break;
            }                                              //break out of loop if user types exit
            else if(strcmp(tokens[0], "list") == 0){
                cmdcol_print(col1);
		continue;
	    }                                              //print all jobs from cmdcol structure using helper function
            else if(strcmp(tokens[0], "pause") == 0){
		long john = 0;				   //init variables for nanoseconds(john) and seconds (jim)
                int jim = 0;
                if(tokens[1] != NULL){
                  john = atol(tokens[1]);		   //convert nano seconds to long if exists
                }
                if(tokens[2] != NULL){
                  jim = atoi(tokens[2]);		   //convert seconds to int if exists
                }
                pause_for(john, jim);
            }                                              //pause for the given number of nanseconds and seconds
            else if(strcmp(tokens[0], "output-for") == 0){
		int job = atoi(tokens[1]); 		   //convert job number to int
		printf("@<<< Output for %s[#%d] (%d bytes):\n",  (col1->cmd[job])->name,  col1->cmd[job]->pid, (col1->cmd[job])->output_size);
		printf("----------------------------------------\n");  //format printing
		cmd_print_output(col1->cmd[job]);                      //helper function to print output from cmd.c
		printf("----------------------------------------\n");  //format printing
            }                                              //print the output for given job number
            else if(strcmp(tokens[0], "output-all") == 0){
                for(int i = 0; i<col1->size; i++){		            //loop through col1 structure to output all jobs
 		   printf("@<<< Output for %s[#%d] (%d bytes):\n",  (col1->cmd[i])->name, col1->cmd[i]->pid, (col1->cmd[i])->output_size);
		   printf("----------------------------------------\n");    //format printing
		   cmd_print_output(col1->cmd[i]);			    ////helper function to print output from cmd.c
		   printf("----------------------------------------\n");    //format printing
		}
            }                                              //print output for all jobs
            else if(strcmp(tokens[0], "wait-for") == 0){
		cmd_update_state(col1->cmd[atoi(tokens[1])],DOBLOCK);
            }                                              //wait for given job to finish
            else if(strcmp(tokens[0], "wait-all") == 0){
                for(int i = 0; i<(col1->size); i++){       //loop through col structure to wait for all jobs
                   cmd_update_state(col1->cmd[i],DOBLOCK);
                }
            }                                              //wait for all jobs to finish
         /*   else if(strcmp(tokens[0], "!") == 0){
                char **rem_toks = &tokens[1];
                cmd_t *newcmd = cmd_new(rem_toks);
                cmdcol_add(col1,newcmd); 		   //add cmd to col structure
                printf("Reporting command '%s' immediately\n", newcmd->name);
                cmd_start(newcmd);		           //start the command
                cmd_update_state(newcmd,DOBLOCK);
                cmd_print_output(newcmd);
            }*/ //exam addition
            else{
		cmd_t *newcmd = cmd_new(tokens);	   //create new cmd structure
		cmdcol_add(col1,newcmd); 		   //add cmd to col structure
		cmd_start(newcmd);		           //start the command
            }                                              //if user enters a new command (not from the help menu)
	    cmdcol_update_state(col1, NOBLOCK);            //update state before looping again
        }
    }
    cmdcol_freeall(col1);
    free(col1);
    return 0;
}
