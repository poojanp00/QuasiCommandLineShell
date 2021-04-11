#include "commando.h"
#include <stdio.h>
#include <string.h>

// cmd.c: functions related the cmd_t struct abstracting a
// command. Most functions maninpulate cmd_t structs.

cmd_t *cmd_new(char *argv[]){
    cmd_t* cmd = malloc(sizeof(cmd_t));              //allocate memory
    char *dup_argv;                                  //duplicate argument vector
    int count = 0;
    while(argv[count] != NULL){
        count ++;
    }                                                //get number of elements in argument vector

    for(int i = 0; i<count; i++){
        dup_argv = strdup(argv[i]);                  //duplicate each argv to dup_argv
        cmd->argv[i] = dup_argv;                     //set argv for running child
    }

    strncpy(cmd->name,argv[0], NAME_MAX);            //initialize name to first argument string
    cmd->argv[count] = NULL;                         //NULL Terminated
    cmd->pid = -1;                                   //PID
    cmd->out_pipe[0] = -1;                           //out_pipe
    cmd->out_pipe[1] = -1;
    cmd->finished = 0;                               //Sets finished to 0 (not finished yet)
    cmd->status = -1;                                //default value -1
    snprintf(cmd->str_status,STATUS_LEN,"INIT");     //str_status
    cmd->output = NULL;                              //default value NULL
    cmd->output_size = -1;                           //default value -1
    return cmd;
}
// Allocates a new cmd_t with the given argv[] array. Makes string
// copies of each of the strings contained within argv[] using
// strdup() as they likely come from a source that will be
// altered. Ensures that cmd->argv[] is ended with NULL. Sets the name
// field to be the argv[0]. Sets finished to 0 (not finished yet). Set
// str_status to be "INIT" using snprintf(). Initializes the remaining
// fields to obvious default values such as -1s, and NULLs.


void cmd_free(cmd_t *cmd){
    for(int i = 0; cmd->argv[i] != NULL; i++){
        free(cmd->argv[i]);                         //free each argv from the struct, break when argv[i] = NULL encountered
    }
    if(cmd->output != NULL){
        free(cmd->output);                          //free output buffer if it is not NULL
    }
    free(cmd);                                      //free cmd itself
    return;
}
// Deallocates a cmd structure. Deallocates the strings in the argv[]
// array. Also deallocats the output buffer if it is not
// NULL. Finally, deallocates cmd itself.

void cmd_start(cmd_t *cmd){
    int result = pipe(cmd->out_pipe);                            //pipe
    if(result != 0){
        perror("fail");
        exit(1);                                                 //error check
    }
    snprintf(cmd->str_status, STATUS_LEN, "RUN");                //change str_status to RUN
    pid_t child = fork();                                        //fork child
    if(child == 0){                                              //CHILD CODE
        dup2(cmd->out_pipe[PWRITE],STDOUT_FILENO);               //redirect output to pipe
        execvp(cmd->name, cmd->argv);                            //execute command
        close(cmd->out_pipe[PREAD]);                             // close read end of pipe
    }
    else{                                                        //PARENT CODE
        cmd->pid = child;                                        //set pid field to child pid
        close(cmd->out_pipe[PWRITE]);                            // close write end of pipe
    }
    return;
}
// Forks a process and executes command in cmd in the process.
// Changes the str_status field to "RUN" using snprintf().  Creates a
// pipe for out_pipe to capture standard output.  In the parent
// process, ensures that the pid field is set to the child PID. In the
// child process, directs standard output to the pipe using the dup2()
// command. For both parent and child, ensures that unused file
// descriptors for the pipe are closed (write in the parent, read in
// the child).


void cmd_update_state(cmd_t *cmd, int block){
    int status;                                                  //status var needed later
    if(cmd->finished == 1){
        return; 			                         //return if the finished flag is 1
    }
    else{
        int pid = waitpid(cmd->pid, &status, block);		 //wait for child, block or dont block passed from main
        if(pid == 0){
          return;						 //no change if pid = 0
        }
	if(pid == -1){
	   snprintf(cmd->str_status,STATUS_LEN,"EXIT(1)");       //error if pid = -1
	}
        else if(pid>0 && WIFEXITED(status) != 0){                //if there is a change...
                cmd->status = WEXITSTATUS(status);               //update status
                snprintf(cmd->str_status,STATUS_LEN,"EXIT(%d)", cmd->status); //change status using snprintf
                cmd->finished = 1;                               //Sets finished to 1
                cmd_fetch_output(cmd);                           //fetch output
		printf("@!!! %s[#%d]: %s \n", cmd->name, cmd->pid, cmd->str_status);//print
        }
    }
    return;
}

// If the finished flag is 1, does nothing. Otherwise, updates the
// state of cmd.  Uses waitpid() and the pid field of command to wait
// selectively for the given process. Passes block (one of DOBLOCK or
// NOBLOCK) to waitpid() to cause either non-blocking or blocking
// waits.  Uses the macro WIFEXITED to check the returned status for
// whether the command has exited. If so, sets the finished field to 1
// and sets the cmd->status field to the exit status of the cmd using
// the WEXITSTATUS macro. Calls cmd_fetch_output() to fill up the
// output buffer for later printing.
//
// When a command finishes (the first time), prints a status update
// message of the form
//
// @!!! ls[#17331]: EXIT(0)
//
// which includes the command name, PID, and exit status.

char *read_all(int fd, int *nread){
    int maxsize = BUFSIZE;                                //start with a max size of 1024
    int mult  = 1;					  //initialize a multiplier, which we will double every time realloc 
    int total = 0;					  //keep track of total bytes
    int bytes_read = 0;                                   //keep track of bytes read per cycle
    char *buf = malloc(sizeof(char)*maxsize);             //malloc initial array
    bytes_read = read(fd, buf, maxsize);                  //read as many as you can, but max of 1024
    total +=bytes_read;                                   //add to total
    while(bytes_read == (maxsize * mult)){                //if we read enough character that we filled the entire buffer, we realloc
	mult *= 2;					  //double multiplier
	char *bigbuf = realloc(buf, (sizeof(char)*(maxsize*mult)));                     //realloc, like we did in class
	bytes_read += read(fd, &bigbuf[bytes_read], ((maxsize*mult)-bytes_read));       //read as many bytes as you can
        buf = bigbuf;                                                                   //update so end of buf points to beginning of bigbuf
    }
    *nread = bytes_read;                                                                //update pointers
    buf[*nread] = '\0';                                                                 //NULL terminate
    return buf;                                                                         //return
}
// Reads all input from the open file descriptor fd. Assumes
// character/text output and null-terminates the character output with
// a '\0' character allowing for printf() to print it later. Stores
// the results in a dynamically allocated buffer which may need to
// grow as more data is read.  Uses an efficient growth scheme such as
// doubling the size of the buffer when additional space is
// needed. Uses realloc() for resizing.  When no data is left in fd,
// sets the integer pointed to by nread to the number of bytes read
// and return a pointer to the allocated buffer. Ensures the return
// string is null-terminated. Does not call close() on the fd as this
// is done elsewhere.

void cmd_fetch_output(cmd_t *cmd){
    if(cmd->finished == 0){
        printf("%s[#%d] not finished yet", cmd->name, cmd->pid);   // if cmd->finished is zero, prints an error message
    }
    else{
        cmd->output = read_all(cmd->out_pipe[PREAD], &cmd->output_size);   //else, read output to cmd->output with read_all
        close(cmd->out_pipe[PREAD]);                                      //close pipe
    }
    return;
}
// If cmd->finished is zero, prints an error message with the format
//
// ls[#12341] not finished yet
//
// Otherwise retrieves output from the cmd->out_pipe and fills
// cmd->output setting cmd->output_size to number of bytes in
// output. Makes use of read_all() to efficiently capture
// output. Closes the pipe associated with the command after reading
// all input.

void cmd_print_output(cmd_t *cmd){
    if(cmd->output == NULL){
        printf("%s[#%d] : output not ready\n", cmd->name, cmd->pid);   // prints an error message
    }
    else{
        write(STDOUT_FILENO, cmd->output, cmd->output_size);           //write to the output field
    }
    return;
}
// Prints the output of the cmd contained in the output field if it is
// non-null. Prints the error message
//
// ls[#17251] : output not ready
//
// if output is NULL. The message includes the command name and PID.

