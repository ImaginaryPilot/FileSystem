#include "trie.h"

void *safeMalloc(size_t n) {
    void *p = malloc(n);
        if (p == NULL) {
            printf("Error: malloc(%ld) failed. Out of memory?\n", n);
            exit(EXIT_FAILURE);
        }
    return p;
}

//function to create files for the trie
file *createFile(char *name, int isDirectory, file *parent){
    file *newnode = (file *)safeMalloc(sizeof(file));
    strcpy(newnode->name, name);
    newnode->isDirectory = isDirectory;
    newnode->parent = parent;

    if (isDirectory) {
        for (int i = 0; i < 50; i++) {
            newnode->content.children[i] = NULL;
        }
    } else {
        newnode->content.data = NULL; // Regular file starts with no content
    }
    return newnode;
}

file *createFileSystem() {
    return createFile("/", 1, NULL);
}

//function that inserts an file to the directory using path
void insertFile(file *target) {
    
}

file *searchFile(file *root, char *path) {
    if (root == NULL || path == NULL) {
        return NULL;
    }

    // If searching for the root directory
    if (strcmp(path, "/") == 0) {
        return root;
    }

    char *pathCopy = strdup(path); // Copy path since strtok modifies it
    char *token = strtok(pathCopy, "/"); // Tokenize by "/"
    file *current = root;

    while (token != NULL) {
        if (!current->isDirectory) { // If it's a regular file, we can't go deeper
            free(pathCopy);
            return NULL;
        }

        int found = 0;
        for (int i = 0; i < 50; i++) {
            if (current->content.children[i] != NULL && strcmp(current->content.children[i]->name, token) == 0) {
                current = current->content.children[i];
                found = 1;
                break;
            }
        }

        if (!found) { // If no match found, return NULL
            free(pathCopy);
            return NULL;
        }

        token = strtok(NULL, "/");
    }

    free(pathCopy);
    return current; // Return the found file/directory
}

int removeFile(file *parent, char *name){
    if (parent == NULL || !parent->isDirectory || parent->parent == NULL) {
        return 0; // Failure
    }
}

//function to free the trie function
void freeTrie(file *root) {
    if(root == NULL) {
        return;
    }
    free(root);
}

 