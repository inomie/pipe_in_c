#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>

/**
 * @author Robin Lundin Söderberg
 * cs:hed20rlg
 * Date: 2021-00-00
 * Description: 
 * 
 * 
 */

int readCommands(char* buff, char* string, int argc, char* argv[]);
int numCommands(char* string, int commandsFrom);

int main(int argc, char *argv[]) {

    char* buff = malloc(1024 * sizeof(*buff) + 1);
    char* string = malloc(1024 * sizeof(*string) + 1);
    if(buff == NULL){
        fprintf(stderr, "Malloc failed");
        exit(EXIT_FAILURE);
    } 

    string[0] = '\0';

    //Get the commands to string[].
    int numOfCommands = readCommands(buff, string, argc, argv);
    

    

    

    printf("%s", string);
    printf("%d", numOfCommands);
    
    

    

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
 * @return Returns 0 = commands from file, 1 = commands from user.
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
            fclose(fp);
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
        
        while (fgets(buff, sizeof(buff), stdin) != NULL){

            
            if(strlen(buff) <= 1024) {
                strcat(string, buff);
            }
            
            
            
        }
        
        return numCommands(string, commandsFrom);
           
    } else {
        
        //Reads stdin from user until ctrl-d, and check if the commands is under 1025 charcters.
        while (fgets(buff, sizeof(buff), stdin) != NULL){

            

            if(strlen(buff) <= 1024) {
                strcat(string, buff);
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
    size_t i = 0;
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