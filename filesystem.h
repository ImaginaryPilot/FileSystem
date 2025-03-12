#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cd(file **currentDir, file *root, char *path);
void ls();
void cat();
void find();
void touch();
void echo();
void mkdir();
void mv();
void cp();
void rm();
void ln();

#endif