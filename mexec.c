#include "mexec_func.h"

/**
 * @author Robin Lundin Söderberg
 * @date: 2021-11-18
 * @version 1
 * Description: This program is working as a pipeline in linux.
 * An pipeline is when you do several commands but the first command
 * send's the out data to the next command and that command takes the data and use it 
 * with it's own command, and so on.
 * An exempel of pipeline in linus is "cat -n mexec.c | grep -B4 -A2 return | less".
 * This program will take a text-file as a parameter with each command on a new line.
 * You can also run the program like "./mexec < file" instead of a file input. 
 * This will take the commands from text-file and store it in stdin buffer.
 * You can also run the program with "./mexec" and type in the command by your self.
 * After entering the commands you press ctrl-d to tell the program to run.
 * 
 * Exampel of a text-file or in data by user:
 * cat -n mexec.c
 * grep -B4 -A2 return
 * less
 */

int main(int argc, char *argv[]) {

    /* Check if more then 2 argument's was given */
    if(argc > 2) {
        fprintf(stderr,"usage: ./mexec [FILE]\n");
        exit(EXIT_FAILURE);
    }

    char* buff;
    /* Controll malloc */
    if((buff = (char *)malloc(MAX_LENGTH * sizeof(char) + 1)) == NULL){
        perror(buff);
        exit(EXIT_FAILURE);
    } 

    char* string;
    /* Controll malloc */
    if((string = (char *)malloc(MAX_LENGTH * sizeof(char) + 1)) == NULL){
        perror(string);
        exit(EXIT_FAILURE);
    }

    /* Add null to the array so strlen and strcat works. */
    string[0] = '\0';

    
    int numOfCommands = 0;

    if (argc == 2) {
        /* Get's the data from an file */
        numOfCommands = readFile(argv, buff, &string);
    } else {
        /* Get's the data from stdin */
        numOfCommands = readStdin(buff, &string);
    }

    if(numOfCommands == 0) {
        fprintf(stderr,"No commands to run");
        free(buff);
        free(string);
        exit(EXIT_FAILURE);
    }
    
    int pids[numOfCommands];
    int numOfPipes = numOfCommands - 1;
    int pipes[numOfPipes][2];
    
    /* Create all the pipes thats needed */
    createPipes(numOfPipes, pipes);
    
    /* Create the child process */
    createChildProcess(numOfCommands, pids, buff, string, numOfPipes, pipes);

    /* Parent code */
    parentProcess(buff, string, numOfPipes, numOfCommands, pipes);
    
    free(buff);
    free(string);

    return 0;
}

