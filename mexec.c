#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>


int readCommands(char* buff, char* string, int argc, char* argv[]);
int numCommands(char* string, int commandsFrom);

int main(int argc, char *argv[]) {

    char* buff = malloc(sizeof(buff) + 1);
    char* string = malloc(sizeof(string) + 1);
    pid_t PID;
    int fd[2];

    //Get the commands to string[].
    int numOfCommands = readCommands(buff, string, argc, argv);

    if(pipe(fd) != 0){
        perror("pipe didnt work");
        exit(EXIT_FAILURE);
    }

    free(buff);
    free(string);

    return 0;
}

/**
 * Reads in all the commands from file, stdin(file) and stdin(terminal).
 * @param buff An array to store the in data on.
 * @param string Where to save all the in data on.
 * @param argc How many arguments was it at start.
 * @param argv Array of all the arguments.
 * @return Returns number of commands.
 */
int readCommands(char* buff, char* string, int argc, char* argv[]){

    //Sets the pointer to the end of stdin buffer.
    fseek(stdin, 0, SEEK_END);

    //0 = file, 1 = user.
    int commandsFrom = 0;

    //Controll the max arguments.
    if(argc > 2) {

        fprintf(stderr,"Too many argument, max two arguments\n");
        exit(EXIT_FAILURE);

    }else if (argc == 2){

        FILE *fp = fopen(argv[1], "r");
        
        if(fp == NULL) {

            fprintf(stderr, "Cant open file");
            exit(EXIT_FAILURE);

        }else {

            while(fgets(buff, sizeof(buff), fp) != NULL) {

                //Check size of the commands, max length is 1024 characters.
                if (strlen(buff) <= 1024) {
                    strncat(string, buff, strlen(buff));
                }      
            }

            return numCommands(string, commandsFrom);
        }

        fclose(fp);

    //Check the file is empty or not.
    }else if(ftell(stdin) == 0) {

        fprintf(stderr, "File is empty");
        exit(EXIT_FAILURE);

    }else if (ftell(stdin) > 0) {
        
        //Set the pointer to the beginning of the stdin buffer.
        fseek(stdin, 0, SEEK_SET);

        //Reads from stdin buffer in to an array.
        fread(buff, sizeof(buff) + 1, 1, stdin);
        int j = 0;
        int k = 0;

        //Checking so the commands is not over 1024 characters.
        while (buff[j] != '\0'){
            if(buff[j] != '\n') {
                k++;
            } else if(k > 1024) {
                fprintf(stderr, "One command is longer then 1024 char");
                exit(EXIT_FAILURE);
            }
            else {
                k = 0;
            }
            j++;
        }

        //Copy the commands over to an array of all the commands.
        strncat(string, buff, strlen(buff));

        return numCommands(string, commandsFrom);
           
    } else {
        
        //Reads stdin from user until ctrl-d, and check if the commands is under 1025 charcters.
        while (fgets(buff, sizeof(buff), stdin) != NULL){

            if(strlen(buff) <= 1024) {
                strncat(string, buff, strlen(buff));
            }
            
        }

        commandsFrom = 1;
        return numCommands(string, commandsFrom);
          
    }

}

/**
 * Counts how many commands that was writen to know how many children it needs.
 * @param string All the commands that was writen.
 * @param commandsFrom 0 = commands from files, 1 = commands from user (terminal).
 * @return Returns an int of how many commands that exist in string[].
 */
int numCommands(char* string, int commandsFrom) {

    int commands = 0;
    int i = 0;
    while(i <= strlen(string)) {

        if(string[i] == '\n') {
            commands++;
        }
        i++;
    }

    //last command ends with '\0' from files and do not have an '\n'.
    if(commandsFrom == 0) {
        commands++;  
    }
    

    return commands;
}