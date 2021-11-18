
/**
 * @file mexec_func.c
 * @author Robin Lundin Söderberg
 * @date: 2021-11-18
 * @version 1
 */

#ifndef _MEXEC_FUNC_H
#define _MEXEC_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_LENGTH 1024

/**
 * This function is the child process and will just be done by the child and not the parent process.
 * @param buff Is an array that's need to be free'd if it fail's.
 * @param string Is an array that's need to be free'd if it fail's.
 * @param numOfPipes Number of pipes.
 * @param numOfCommands Number of commands.
 * @param pipes All the pipes that was created. 
 * 
 */
void childProcess(char *buff, char *string, int i, int numOfCommands, int numOfPipes, int pipes[][2]);

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
void parentProcess(char *buff, char *string, int numOfPipes, int numOfCommands, int pipes[][2]);

/**
 * This function will be done by the parent process that will wait for
 * the child processes to be done and se if they fail'd or not.
 * @param numOfCommands Number of commands.
 * @param buff Is an array that's need to be free'd if wait fail's.
 * @param string Is an array that's need to be free'd if wait fail's.
 * 
 */
void parentWait(int numOfCommands, char *buff, char *string);

/**
 * The function devides the array up in to strings and then execute the commands.
 * @param command An two dimansional array with all the commands.
 * @param string An array of commands, need to be free'd if fail.
 * @param buff An array used to read in from indata, need to be free'd if failed.
 * @param i The number of the child.
 * 
 */
void makeString(char command[][MAX_LENGTH], char *string, char *buff, int i);

/**
 * The function will go through the array of command and save down the correct
 * command for that child in to a new array.
 * @param command Will store the command that the child will exec.
 * @param string Array of all the commands.
 * @param child An number of which child it is.
 * 
 */
void childCommand(char command[][MAX_LENGTH], char* string);

/**
 * Function is moving the correct pipe to stdin or stdout.
 * @param i Number of the child.
 * @param pipes An two dimensional array with the pipes.
 * @param numOfCommands Number of how many command it is.
 * 
 */
void dupPipes(int i, int pipes[][2], int numOfCommands);

/**
 * This function takes care of the reading from file.
 * @param argv Is the argument vector. Contains all the arguments.
 * @param buff An array to store the data from stdin/file.
 * @param string An array to save down all the data to.
 * @return Returns the number of rows/commands.
 * 
 */
int readFile(char *argv[], char *buff, char **string);

/**
 * This function takes care of the reading from stdin.
 * @param argv Is the argument vector. Contains all the arguments.
 * @param buff An array to store the data from stdin/file.
 * @param string An array to save down all the data to.
 * @return Returns the number of rows/commands.
 * 
 */
int readStdin(char *buff, char **string);

/**
 * 
 * This function saves the commands to an array.
 * If the command's is longer then 1024 it realloc the memory.
 * @param size Is the size that string need's to be.
 * @param buff An array to store the data from stdin/file.
 * @param string An array to save down all the data to.
 * @return Returns the size that string need's to be.
 */
int saveData(char **string, char *buff, int size);

/**
 * This function will create all the pipes and check that no one fail'd.
 * @param numOfPipes Is the numer of pipes that need's to be created.
 * @param pipes Is an two dimensional array for each pipe
 * it have read and write.
 * 
 */
void createPipes(int numOfPipes, int pipes[][2]);

/**
 * This function will execute the commands.
 * @param arr array of string's with commands.
 * @param buff An array that need's to be free'd if it fail's.
 * @param string An array that need's to be free'd if it fail's.
 * 
 */
void exec(char *arr[], char *buff, char *string);

/**
 * This function create's all the child processes that is needed with help of fork().
 * @param numOfCommands Number of commands.
 * @param pids An array with all pid numbers.
 * @param buff Is an array that's need to be free'd if it fail's.
 * @param string Is an array that's need to be free'd if it fail's.
 * @param numOfPipes Number of pipes.
 * @param pipes All the pipes that was created. 
 */
void createChildProcess(int numOfCommands, int pids[], char *buff, char *string , int numOfPipes, int pipes[][2]);


#endif