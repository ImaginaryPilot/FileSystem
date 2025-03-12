#include "filesystem.h"
#include "createFile.h"

void cd(file **currentDir, file *root, char *path){
    if (path == NULL || strcmp(path, "/") == 0) {
        // If the path is NULL or "/", go to the root directory
        *currentDir = root;
        return;
    }

    if (strcmp(path, "..") == 0) {
        // If the user enters "..", go to the parent directory
        if ((*currentDir)->parent != NULL) {
            *currentDir = (*currentDir)->parent;
        }
        return;
    }

    // Try to find the directory by using searchFile
    file *newDir = searchFile((*currentDir), path);
    if (newDir != NULL && newDir->isDirectory) {
        // If the directory exists and it's a directory, change the currentDir
        *currentDir = newDir;
    }
}

void ls(){
    printf("ls");
}

void cat(file *currentDir, char *path){
    file *newDir = searchFile(currentDir, path);
    printf("%s\n", newDir->content.data);
}

void find(){
    printf("find");
}

void touch(file *currentDir, char *filename) {
    char *token = strtok(filename, " ");  // Get the first filename

    while (token != NULL) {
        // Create an empty file in the current directory
        insertFile(currentDir, token, 0, NULL);
        token = strtok(NULL, " ");  // Get the next filename
    }
}

void echo(){
    printf("echo");
}

void mkdir(){
    printf("mkdir");
}

void mv(){
    printf("mv");
}

void cp(){
    printf("cp");
}

void rm(){
    printf("rm");
}

void ln(){
    printf("ln");
}