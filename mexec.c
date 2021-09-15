#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

/**
 * @author Robin Lundin Söderberg
 * cs:hed20rlg
 * Date: 2021-09-15
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
 * 
 */

int numCommands(char* string, int commandsFrom);
void childCommand(char command[][1024], char* string);

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

    /*---Function-(Cant be in function. Realloc are not work in function, can be a problem to realloc a char *. Maybe need char **)---*/
    /*---------------------------------------Read in commands from stdin or file------------------------------------------------------*/

    //Sets the pointer to the end of stdin buffer.
    fseek(stdin, 0, SEEK_END);

    //0 = file, 1 = user.
    int commandsFrom = 0;

    //Controll the max arguments.
    if(argc > 2) {

        fprintf(stderr,"usage: ./mexec [FILE]\n");
        free(buff);
        free(string);
        exit(EXIT_FAILURE);

    }else if (argc == 2){

        FILE *fp = fopen(argv[1], "r");
        
        if(fp == NULL) {

            perror(argv[1]);
            free(buff);
            free(string);
            exit(EXIT_FAILURE);

        }else {
            
            int loops = 0;
            while(fgets(buff, sizeof(buff), fp) != NULL) {

                loops++;
                int count = loops * 1000;
            
                if(loops > 1) {
                    //Reallocate string array for each row.
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
            //Controll how many numbers of commands/rows.
            numOfCommands = numCommands(string, commandsFrom);
        }

        fclose(fp);

    //Check the stdin is empty or not.
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
                //Reallocate string array for each row.
                string = realloc(string, (count*sizeof(char) + 1));
                if(string == NULL) {
                    fprintf(stderr, "Realloc fail");
                    exit(EXIT_FAILURE);
                } 
            }
            //Check size of the commands, max length is 1024 characters.
            if(strlen(buff) < 1024) {
                strcat(string, buff);
            } else{
                fprintf(stderr, "One command is to long");
                exit(EXIT_FAILURE);
            } 
        }
        //Controll how many numbers of commands/rows.
        numOfCommands = numCommands(string, commandsFrom);
           
    } else {
        
        int loops = 0;
        //Reads stdin from user until ctrl-d, and check if the commands is under 1025 charcters.
        while (fgets(buff, sizeof(buff), stdin) != NULL){

            loops++;
            int count = loops * 1000;
            
            if(loops > 1) {
                //Reallocate string array for each row.
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
        //Set commandFrom to 1 so the function know that the data is from stdin/user.
        commandsFrom = 1;
        //Controll how many numbers of commands/rows.
        numOfCommands = numCommands(string, commandsFrom);   
    }
    
    /*------------------------------------------------------------------------------------------------------------------*/

    int pids[numOfCommands];
    int numOfPipes = numOfCommands - 1;
    int pipes[numOfPipes][2];
    
    //Open pipes
    for (int i = 0; i < numOfPipes; i++){
        if(pipe(pipes[i]) < 0){
            perror("Failed to pipe\n");
            exit(EXIT_FAILURE);
        }
    }
    
    //Loop fork as many times commands. children will exit with exec so they dont loop.
    for (int i = 0; i < numOfCommands; i++)
    {
        pids[i] = fork();

        if(pids[i] < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if(pids[i] == 0){
            /*Child code,
              Close pipes */
            for (int j = 0; j < numOfPipes; j++)
            {
                if(i != j) {
                    close(pipes[j][1]);
                }
                if(i - 1 != j) {
                    close(pipes[j][0]);
                } 
            }

            

            if(numOfCommands > 1) {
                
                /* Dup pipe to STDIN_FILENO or STDOUT_FILENO */
                if(i == 0){
                    /* Only the first child will do this */
                    if (dup2(pipes[i][1], STDOUT_FILENO) < 0) {
                        perror("dup on first child failed");
                        close(pipes[i][1]);
                        exit(EXIT_FAILURE);
                    }  
                } else if (i == (numOfCommands - 1)){
                    /* Only the last child will do this */
                    if (dup2(pipes[i - 1][0], STDIN_FILENO) < 0) {
                        perror("dup on last child failed");
                        close(pipes[i - 1][0]);
                        exit(EXIT_FAILURE);
                    }  
                } else {
                    /* All the other children will do this */
                    if (dup2(pipes[i - 1][0], STDIN_FILENO) < 0) {
                        perror("dup STDIN_FILENO on middle children failed");
                        close(pipes[i - 1][0]);
                        exit(EXIT_FAILURE);
                    }
                    if (dup2(pipes[i][1], STDOUT_FILENO) < 0) {
                        perror("dup STDOUT_FILENO on middle children failed");
                        close(pipes[i][1]);
                        exit(EXIT_FAILURE);
                    }
                }
            }
            
            
            /* Get the command for the right child */
            char command[numOfCommands][1024];
            childCommand(command, string);
            

            /* Split the array in to strings with strtok.
                Dont use strtok if you dont know what's happening with the memory */
            char *arr[] = {NULL};
            
            command[i][strlen(command[i]) + 1] = '\0';
            
            char *t = command[i];

            char *token;
            int index = 0;
            
            //printf("%s\n", arr[index]);
            
            while(1){
                if(t != NULL) {
                    token = strtok_r(t, " ", &t);
                }
                if(token != NULL) {
                    arr[index] = token;
                    index++;
                } else{
                    arr[index] = NULL;
                    if(execvp(arr[0], arr) < 0) {
                        perror(arr[0]);
                        free(buff);
                        free(string);
                        exit(EXIT_FAILURE);
                    }
                    exit(EXIT_FAILURE);
                }
                
            }

            
            
            free(buff);
            free(string);
            exit(EXIT_FAILURE);
            
        }
    }

    /*-------------Parent code------------------------*/

    /* Close all the pipes */
    for (int i = 0; i < numOfPipes; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    /* Wait for all the children to be done */
    for (int j = 0; j < numOfCommands; j++)
    {
        int controll = wait(NULL);
        printf("%d\n", controll);
        if(controll < 0){
            free(buff);
            free(string);
            exit(EXIT_FAILURE);
        }
    } 
    
    free(buff);
    free(string);

    return 0;
}

/**
 * Counts how many commands/rows that was writen to know how many children it needs.
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

/**
 * The function will go through the array of command and save down the correct
 * command for that child in to a new array.
 * @param command Will store the command that the child will exec.
 * @param string Array of all the commands.
 * @param child An number of which child it is.
 */
void childCommand(char command[][1024], char* string) {
    int commandNum = 0;
    int index = 0;

    /* Loop through the string array */
    for (size_t i = 0; i <= strlen(string); i++)
    {
        if(string[i] == '\n' || string[i] == '\0') {
            command[commandNum][index] = '\0';
            commandNum++;
            index = 0;
        }else {
           command[commandNum][index] = string[i]; 
           index++;
        } 
    }
}