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


int numCommands(char* string, int commandsFrom);
void childCommand(char* command, char* string, int child);

int main(int argc, char *argv[]) {

    char* buff = (char *)malloc(1024 * sizeof(char) + 1);
    char* string = (char *)malloc(1024 * sizeof(char) + 1);
    if(buff == NULL || string == NULL){
        fprintf(stderr, "Malloc failed");
        exit(EXIT_FAILURE);
    } 

    
    
    //Add null to the array so strlen and strcat works.
    string[0] = '\0';
    
    //Get the commands to string[].
    int numOfCommands = 0;

    //-----------------------------------Function (krånglar med realloc i en funktion)------------------------------------------------------------------

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
            
            int loops = 0;
            while(fgets(buff, sizeof(buff), fp) != NULL) {

                loops++;
                int count = loops * 1000;
            
                if(loops > 1) {
                    string = realloc(string, (count*sizeof(char) + 1));
                    if(string == NULL) {
                        fprintf(stderr, "Realloc fail");
                        exit(EXIT_FAILURE);
                    }
                }
                //Check size of the commands, max length is 1024 characters.
                if (strlen(buff) < 1024) {
                    strncat(string, buff, strlen(buff));
                } else{
                    fprintf(stderr, "One command is to long");
                    exit(EXIT_FAILURE);
                }
                
            }
            
            numOfCommands = numCommands(string, commandsFrom);
        }

        fclose(fp);

    //Check the file is empty or not.
    }else if(ftell(stdin) == 0) {

        fprintf(stderr, "File is empty");
        exit(EXIT_FAILURE);

    }else if (ftell(stdin) > 0) {
        
        //Set the pointer to the beginning of the stdin buffer.
        fseek(stdin, 0, SEEK_SET);
        int loops = 0;
        while (fgets(buff, sizeof(buff), stdin) != NULL){

            
            loops++;
            int count = loops * 1000;
            
            if(loops > 1) {
                string = realloc(string, (count*sizeof(char) + 1));
                if(string == NULL) {
                    fprintf(stderr, "Realloc fail");
                    exit(EXIT_FAILURE);
                }
                
            }
            if(strlen(buff) < 1024) {
                strcat(string, buff);
            } else{
                fprintf(stderr, "One command is to long");
                exit(EXIT_FAILURE);
            }
             
        }
        
        numOfCommands = numCommands(string, commandsFrom);
           
    } else {
        
        int loops = 0;
        //Reads stdin from user until ctrl-d, and check if the commands is under 1025 charcters.
        while (fgets(buff, sizeof(buff), stdin) != NULL){

            
            loops++;
            int count = loops * 1000;
            
            if(loops > 1) {
                string = realloc(string, (count*sizeof(char) + 1));
                if(string == NULL) {
                    fprintf(stderr, "Realloc fail");
                    exit(EXIT_FAILURE);
                }
                
            }
            if(strlen(buff) < 1024) {
                strcat(string, buff);
            } else{
                fprintf(stderr, "One command is to long");
                exit(EXIT_FAILURE);
            }
            
        }
        
        commandsFrom = 1;
        numOfCommands = numCommands(string, commandsFrom);
          
    }
    
    //------------------------------------------------------------------------------------------------------------------


    //Array for the child to use.
    char *command = malloc(1024 * sizeof(char) + 1);
    int child = 1;
    
    childCommand(command, string, child);
    
    
    printf("%s\n", command);
    //printf("%s\n", string);
    printf("%d\n", numOfCommands);
    
    

    

    free(buff);
    free(string);
    free(command);

    return 0;
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

void childCommand(char* command, char* string, int child) {
    int k = 0;
    int h = 0;
    for (size_t i = 0; i <= strlen(string); i++)
    {
        if(string[i] == '\n' || string[i] == '\0') {
            if(k == child){
                command[h] = '\0';
                break;
            } else {
                if(k < 1){
                    for (size_t j = 0; j < i; j++)
                    {
                        command[j] = '\0';
                    }
                    
                } else{
                    for (size_t j = 0; j < (i - h); j++)
                    {
                        command[j] = '\0';
                    }
                    
                }
                h = 0;
            }
            k++;
        }
        command[h] = string[i];
        h++;
    }
}