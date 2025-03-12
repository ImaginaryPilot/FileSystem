#ifndef TRIE_H
#define TRIE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//struct for directory
typedef struct file {
    char name[60];
    bool isDirectory;
    struct file *parent;
    int referenceCount;
    int numChild;
    union {
        struct file *children[50]; // For directories: array of child pointers
        char *data; // For regular files: stores file content (ASCII string)
    } content;
} file;

typedef struct fileSys{
    file *root;
    file *active;
}fileSys;

file *createFile(char *name, int isDirectory, file *parent);
file *createFileSystem();


#endif