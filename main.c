#include <stdlib.h> 
#include <stdio.h>
#include <assert.h>
#include "createFile.h"
#include "filesystem.h"

//function that returns a number based on which command is inputted
int getCommandCode(const char *command) {
    if (strcmp(command, "exit") == 0) return 0;
    if (strcmp(command, "ls") == 0) return 1;
    if (strcmp(command, "cd") == 0) return 2;
    if (strcmp(command, "cat") == 0) return 3;
    if (strcmp(command, "find") == 0) return 4;
    if (strcmp(command, "touch") == 0) return 5;
    if (strcmp(command, "echo") == 0) return 6;
    if (strcmp(command, "mkdir") == 0) return 7;
    if (strcmp(command, "mv") == 0) return 8;
    if (strcmp(command, "cp") == 0) return 9;
    if (strcmp(command, "rm") == 0) return 10;
    if (strcmp(command, "ln") == 0) return 11;
    //return -1 if the command is unknown
    return -1;
}

//function that handles command
void handleCommand(fileSys **files, char *commandLine, int *inputFlag){
    char commandCopy[1024];  // No need for malloc, use a fixed-size array
    strncpy(commandCopy, commandLine, sizeof(commandCopy) - 1);  // Copy the commandLine to commandCopy

    // Ensure null-termination just in case strncpy doesn't null-terminate
    commandCopy[sizeof(commandCopy) - 1] = '\0';
    char *token = strtok(commandCopy, " ");  // First token is the command name

    char *restOfCommand = strtok(NULL, "\n");

    if (token == NULL) {
        return;
    }

    int commandCode = getCommandCode(token);
    //the switch calls the function that matches the command
    switch (commandCode) {
        case 0:  
            (*inputFlag) = 0;
            break;
        case 1: 
            ls(files, restOfCommand);  
            break;
        case 2:  
            cd(files, restOfCommand);  
            break;
        case 3:  
            cat(files, restOfCommand);  
            break;
        case 4:  
            find((*files)->active, ".");  
            break;
        case 5:  
            touch(files, restOfCommand);  
            break;
        case 6: 
            echo(files, restOfCommand); 
            break;
        case 7: 
            mkdir(files, restOfCommand);  
            break;
        case 8: 
            mv(files, restOfCommand);  
            break;
        case 9:  
            cp(files, restOfCommand);  
            break;
        case 10: 
            rm(files, restOfCommand); 
            break;
        case 11:  
            ln(files, restOfCommand); 
            break;
        default:
            printf("Unknown command: %s\n", token);
            break;
    }
}

int main(int argc, char *argv[]) {
    // Creating a simple file system
    fileSys *fileSys = initFiles();
    char commandLine[1024];
    int inputFlag = 1;
    while(inputFlag){
        if (fgets(commandLine, sizeof(commandLine), stdin) == NULL) break;
        // Remove the trailing newline character
        commandLine[strcspn(commandLine, "\n")] = 0;
        //skip empty lines
        if (commandLine[0] == '\0') {
            continue;  
        }
        // Handle the command
        handleCommand(&fileSys, commandLine, &inputFlag);
    }
    freeFile(fileSys->root);
    free(fileSys);
    return 0;
}