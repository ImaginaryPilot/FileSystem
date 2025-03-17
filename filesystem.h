#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "createFile.h"

void cd(fileSys **files, char *path);
void ls(fileSys **files, char* path);
void cat(fileSys **files, char *path);
void find(file *root, char* path);
void touch(fileSys **files, char *path);
void echo(fileSys **files, char *input);
void mkdir(fileSys **files, char *path);
void mv(fileSys **files, char *path);
void cp(fileSys **files, char *path);
void rm(fileSys **files, char *path);
void ln(fileSys **fs, char *input);

#endif