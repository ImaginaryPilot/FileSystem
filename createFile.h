#ifndef CREATEFILE_H
#define CREATEFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//structure that helps with handling inodes
typedef struct inode {
    char data[10000]; 
    int referenceCount;            
    struct file *links[256];       
} inode;

//structure for directory
typedef struct file {
    char name[60];
    int isDirectory;
    struct file *parent;
    struct inode *inode;
    int numChild;
    union {
        // For directories: array of child pointers
        struct file *children[256]; 
        // For regular files: stores file content (ASCII string)
        char data[10000]; 
    } content;
} file;

//structure for the entire file system
typedef struct fileSys{
    file *root;
    file *active;
} fileSys;

//declaration of functions in createFile.c
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
#endif