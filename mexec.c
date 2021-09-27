#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

/**
 * @author Robin Lundin Söderberg
 * cs:hed20rlg
 * Date: 2021-09-16
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

#define MAX_LENGTH 1024

void childProcess(char *buff, char *string, int i, int numOfCommands, int numOfPipes, int pipes[][2]);
void parentProcess(char *buff, char *string, int numOfPipes, int numOfCommands, int pipes[][2]);
void parentWait(int numOfCommands, char *buff, char *string);
void makeString(char command[][MAX_LENGTH], char *string, char *buff, int i);
void childCommand(char command[][MAX_LENGTH], char* string);
void dupPipes(int i, int pipes[][2], int numOfCommands);
int readFile(char *argv[], char *buff, char **string);
int readStdin(char *buff, char **string);
int saveData(char **string, char *buff, int size);
void createPipes(int numOfPipes, int pipes[][2]);
void exec(char *arr[], char *buff, char *string);
void createChildProcess(int numOfCommands, int pids[], char *buff, char *string , int numOfPipes, int pipes[][2]);

int main(int argc, char *argv[]) {

    char* buff = (char *)malloc(MAX_LENGTH * sizeof(char) + 1);
    /* Controll malloc */
    if(buff == NULL){
        perror(buff);
        exit(EXIT_FAILURE);
    } 

    char* string = (char *)malloc(MAX_LENGTH * sizeof(char) + 1);
    /* Controll malloc */
    if(string == NULL){
        perror(string);
        exit(EXIT_FAILURE);
    }

    /* Add null to the array so strlen and strcat works. */
    string[0] = '\0';

    
    int numOfCommands = 0;

    /* Check if more then 2 argument's was given */
    if(argc > 2) {
        fprintf(stderr,"usage: ./mexec [FILE]\n");
        free(buff);
        free(string);
        exit(EXIT_FAILURE);
    }else if (argc == 2) {
        /* Get's the data from an file */
        numOfCommands = readFile(argv, buff, &string);
    } else {
        /* Get's the data from stdin */
        numOfCommands = readStdin(buff, &string);
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

/**
 * This function takes care of the reading from file.
 * @param argv Is the argument vector. Contains all the arguments.
 * @param buff An array to store the data from stdin/file.
 * @param string An array to save down all the data to.
 * @return Returns the number of rows/commands.
 * 
 */
int readFile(char *argv[], char *buff, char **string) {

    FILE *fp = fopen(argv[1], "r");
    
    if(fp == NULL) {
        perror(argv[1]);
        free(buff);
        free(*string);
        exit(EXIT_FAILURE);
    }else {
        int rows = 0;
        int size = MAX_LENGTH;
        while(fgets(buff, MAX_LENGTH, fp) != NULL) {
            rows++;
            size = saveData(string, buff, size);
        }
        //Return how many lines.
        fclose(fp);
        return rows;
    }
}

/**
 * This function takes care of the reading from stdin.
 * @param argv Is the argument vector. Contains all the arguments.
 * @param buff An array to store the data from stdin/file.
 * @param string An array to save down all the data to.
 * @return Returns the number of rows/commands.
 * 
 */
int readStdin(char *buff, char **string) {
    //Set the pointer to the end of the stdin buffer.
    fseek(stdin, 0, SEEK_END);
    //Check the stdin is empty or not when < is used.
    if(ftell(stdin) == 0) {
        fprintf(stderr, "File is empty");
        exit(EXIT_FAILURE);
    }else  {
        //Set the pointer to the beginning of the stdin buffer.
        fseek(stdin, 0, SEEK_SET);
        int rows = 0;
        int size = MAX_LENGTH;
        while (fgets(buff, MAX_LENGTH, stdin) != NULL){
            rows++;
            size = saveData(string, buff, size);    
        }    
        //Return how many lines.
        return rows;     
    } 
}

/**
 * This function will create all the pipes and check that no one fail'd.
 * @param numOfPipes Is the numer of pipes that need's to be created.
 * @param pipes Is an two dimensional array for each pipe
 * it have read and write.
 * 
 */
void createPipes(int numOfPipes, int pipes[][2]) {
    for (int i = 0; i < numOfPipes; i++){
        if(pipe(pipes[i]) < 0){
            perror("Failed to pipe\n");
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * This function create's all the child processes that is needed with help of fork().
 * @param numOfCommands Number of commands.
 * @param pids An array with all pid numbers.
 * @param buff Is an array that's need to be free'd if it fail's.
 * @param string Is an array that's need to be free'd if it fail's.
 * @param numOfPipes Number of pipes.
 * @param pipes All the pipes that was created. 
 */
void createChildProcess(int numOfCommands, int pids[], char *buff, char *string , int numOfPipes, int pipes[][2]) {
    /* Loop fork as many times as commands.
        Children will exit with exec so they dont loop. */
    for (int i = 0; i < numOfCommands; i++)
    {
        pids[i] = fork();

        if(pids[i] < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if(pids[i] == 0){
            /* Child code */
            childProcess(buff, string, i, numOfCommands, numOfPipes, pipes);
        }
    }
}

/**
 * This function is the child process and will just be done by the child and not the parent process.
 * @param buff Is an array that's need to be free'd if it fail's.
 * @param string Is an array that's need to be free'd if it fail's.
 * @param numOfPipes Number of pipes.
 * @param numOfCommands Number of commands.
 * @param pipes All the pipes that was created. 
 * 
 */
void childProcess(char *buff, char *string, int i, int numOfCommands, int numOfPipes, int pipes[][2]) {
    /* Close the pipes that will not be used */
    for (int j = 0; j < numOfPipes; j++)
    {
        if(i != j) {
            close(pipes[j][1]);
        }
        if(i - 1 != j) {
            close(pipes[j][0]);
        } 
    }

    /* Dup the pipes to correct out and in if it's more then one command */
    if(numOfCommands > 1) {
        dupPipes(i, pipes, numOfCommands);
    }
    
    /* The command to exec */
    char command[numOfCommands][MAX_LENGTH];

    /* Devide the strings up */
    childCommand(command, string);
    
    command[i][strlen(command[i]) + 1] = '\0';
    
    /* Split array in to strings */
    makeString(command, string, buff, i);

    /* This will never happen but just in case*/
    free(buff);
    free(string);
    exit(EXIT_FAILURE);
}

/**
 * This function is the parent process, This function will only the parent do.
 * The function will close the pipes and wait for the child process to be done.
 * @param buff Is an array that's need to be free'd if wait fail's.
 * @param string Is an array that's need to be free'd if wait fail's.
 * @param numOfPipes Number of pipes to close.
 * @param numOfCommands Number of commands.
 * @param pipes All the pipes that was created.
 * 
 */
void parentProcess(char *buff, char *string, int numOfPipes, int numOfCommands, int pipes[][2]) {
    /* Close all the pipes */
    for (int i = 0; i < numOfPipes; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    /* Wait for all the children to be done */
    parentWait(numOfCommands, buff, string);
}

/**
 * This function will be done by the parent process that will wait for
 * the child processes to be done and se if they fail'd or not.
 * @param numOfCommands Number of commands.
 * @param buff Is an array that's need to be free'd if wait fail's.
 * @param string Is an array that's need to be free'd if wait fail's.
 * 
 */
void parentWait(int numOfCommands, char *buff, char *string) {
    int stat;
    for (int j = 0; j < numOfCommands; j++){
        wait(&stat);
        
        if(stat > 0){
            free(buff);
            free(string);
            exit(EXIT_FAILURE);
        }
    }
}

/**
 * 
 * This function saves the commands to an array.
 * If the command's is longer then 1024 it realloc the memory.
 * @param size Is the size that string need's to be.
 * @param buff An array to store the data from stdin/file.
 * @param string An array to save down all the data to.
 * @return Returns the size that string need's to be.
 */
int saveData(char **string, char *buff, int size) {
    /* Realloc if (string + buff) equals or bigger then size */
    if((strlen(*string) + strlen(buff)) > size) {
        size += MAX_LENGTH;
        fprintf(stderr, "hej\n");
        *string = realloc(*string, (size*sizeof(*string) + 1));
        if(*string == NULL) {
            fprintf(stderr, "Realloc fail");
            exit(EXIT_FAILURE);
        } 
    }
    //Check size of the commands, max length is 1024 characters.
    if(strlen(buff) <= MAX_LENGTH) {
        strcat(*string, buff);
    } else{
        fprintf(stderr, "One command is to long");
        exit(EXIT_FAILURE);
    }

    return size;
}

/**
 * Function is moving the correct pipe to stdin or stdout.
 * @param i Number of the child.
 * @param pipes An two dimensional array with the pipes.
 * @param numOfCommands Number of how many command it is.
 * 
 */
void dupPipes(int i, int pipes[][2], int numOfCommands) {
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

/**
 * The function devides the array up in to strings and then execute the commands.
 * @param command An two dimansional array with all the commands.
 * @param string An array of commands, need to be free'd if fail.
 * @param buff An array used to read in from indata, need to be free'd if failed.
 * @param i The number of the child.
 * 
 */
void makeString(char command[][MAX_LENGTH], char *string, char *buff, int i) {
    
    /* Split the array in to strings with strtok.
        Dont use strtok if you dont know what's happening with the memory */
    char *arr[MAX_LENGTH];
    char *t = command[i];
    char *token;
    int index = 0;
    
    //printf("%s\n", arr[index]);
    token = strtok(t, " ");
    while(1){  
        if(token != NULL) {
            arr[index] = token;
            index++;
        } else{
            arr[index] = NULL;
            exec(arr, buff, string);
              
        }
        token = strtok(NULL, " ");    
    }
}

/**
 * This function will execute the commands.
 * @param arr array of string's with commands.
 * @param buff An array that need's to be free'd if it fail's.
 * @param string An array that need's to be free'd if it fail's.
 * 
 */
void exec(char *arr[], char *buff, char *string) {
    if(execvp(arr[0], arr) < 0) {
        perror(arr[0]);
        free(buff);
        free(string);
        exit(EXIT_FAILURE);
    }
}

/**
 * The function will go through the array of command and save down the correct
 * command for that child in to a new array.
 * @param command Will store the command that the child will exec.
 * @param string Array of all the commands.
 * @param child An number of which child it is.
 * 
 */
void childCommand(char command[][MAX_LENGTH], char* string) {
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