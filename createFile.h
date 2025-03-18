#ifndef TRIE_H
#define TRIE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct inode {
    char data[10000]; // For regular files: stores file content (ASCII string)
    int referenceCount;            // Number of hard links to this inode
    struct file *links[256];        // Array of pointers to file structures that link to this inode
} inode;

//struct for directory
typedef struct file {
    char name[60];
    int isDirectory;
    struct file *parent;
    struct inode *inode;
    int numChild;
    union {
        struct file *children[256]; // For directories: array of child pointers
        char data[10000]; // For regular files: stores file content (ASCII string)
    } content;
} file;

typedef struct fileSys{
    file *root;
    file *active;
} fileSys;

void *safeMalloc(size_t n);
char **makeCharArray2D(int height, int width);
void destroyArray2D(char **arr, int height);
fileSys *initFiles();
file *createFile(char *name, int isDirectory, file *parent, char *data, inode *inodeRef);
void changeActiveDirectory(fileSys **files, file *newDir);
int compareFiles(const void *a, const void *b);
file *searchFile(fileSys **files, file *root, char *path);
void freeFile(file *root);
void copyDirectory(file *source, file *destination);
void removeFile(file *root, char *path);
void printFileSystem(file *root, int depth);
#endif