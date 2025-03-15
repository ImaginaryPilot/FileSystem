#include <stdlib.h> 
#include <stdio.h>
#include <assert.h>
#include "createFile.h"
#include "filesystem.h"

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

    return -1;  // Unknown command
}

void handleCommand(fileSys **files, char *commandLine, int *inputFlag){
    char commandCopy[256];  // No need for malloc, use a fixed-size array
    strncpy(commandCopy, commandLine, sizeof(commandCopy) - 1);  // Copy the commandLine to commandCopy

    // Ensure null-termination just in case strncpy doesn't null-terminate
    commandCopy[sizeof(commandCopy) - 1] = '\0';
    char *token = strtok(commandCopy, " ");  // First token is the command name

    char *restOfCommand = strtok(NULL, "\n");

    if (token == NULL) {
        return;
    }

    int commandCode = getCommandCode(token);

    switch (commandCode) {
        case 0:  // exit
            (*inputFlag) = 0;
            break;
        case 1:  // ls
            ls((*files)->active);  // Assuming ls() doesn't need arguments
            break;
        case 2:  // cd
            cd(files, restOfCommand);  // Assuming cd() doesn't need arguments
            break;
        case 3:  // cat
            cat(files, restOfCommand);  // Assuming cat() doesn't need arguments
            break;
        case 4:  // find
            find((*files)->active, ".");  // Assuming find() doesn't need arguments
            break;
        case 5:  // touch
            touch(files, restOfCommand);  // Assuming touch() doesn't need arguments
            break;
        case 6:  // echo
            echo(files, restOfCommand);  // Assuming echo() doesn't need arguments
            break;
        case 7:  // mkdir
            mkdir(files, restOfCommand);  // Assuming mkdir() doesn't need arguments
            break;
        case 8:  // mv
            mv(files, restOfCommand);  // Assuming mv() doesn't need arguments
            break;
        case 9:  // cp
            cp();  // Assuming cp() doesn't need arguments
            break;
        case 10:  // rm
            rm(files, restOfCommand);  // Assuming rm() doesn't need arguments
            break;
        case 11:  // ln
            ln();  // Assuming ln() doesn't need arguments
            break;
        default:
            printf("Unknown command: %s\n", token);
            break;
    }
}

int main(int argc, char *argv[]) {
    // Creating a simple file system
    fileSys *fileSys = initFiles();
    char commandLine[256];
    int inputFlag = 1;

    while(inputFlag){
        if (fgets(commandLine, sizeof(commandLine), stdin) == NULL) break;

        // Remove the trailing newline character
        commandLine[strcspn(commandLine, "\n")] = 0;

        if (commandLine[0] == '\0') {
            continue;  // Skip empty lines
        }

        // Handle the command
        handleCommand(&fileSys, commandLine, &inputFlag);
    }

    printFileSystem(fileSys->root, 3);

    freeFile(fileSys->root);
    free(fileSys);
    return 0;
}