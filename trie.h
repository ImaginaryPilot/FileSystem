#ifndef TRIE_H
#define TRIE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//struct for directory
typedef struct file {
    char name[60];
    int isDirectory;
    struct file *parent;
    // Inode reference;    
    union {
        struct file *children[50]; // For directories: array of child pointers
        char *data; // For regular files: stores file content (ASCII string)
    } content;
} file;

file *createFile(char *name, int isDirectory, file *parent, char *data);
file *createFileSystem();
file *searchFile(file *root, char *path);
void insertFile(file *root, char *path, int isDirectory, char *data);
void removeFile(file *parent, char *path);
void freeFile(file *root);
void printFileSystem(file *root, int depth);

#endif