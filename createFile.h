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
} fileSys;

void safemalloc(size_t n);
char **makeCharArray2D(int height, int width);
void destroyArray2D(char **arr, int height);
fileSys *initFiles();
file *createFile(char *name, int isDirectory, file *parent, char *data);
void changeActiveDirectory(fileSys **files, file *newDir);
int compareFiles(const void *a, const void *b);
file *searchFile(file *root, char *path);
void insertFile(file *root, char *path, int isDirectory, char *data);
void freeFile(file *root);
void removeFile(file *root, char *path);
void printFileSystem(file *root, int depth);
#endif