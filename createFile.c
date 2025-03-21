#include "createFile.h"

void *safeMalloc(size_t n) {
    void *p = malloc(n);
        if (p == NULL) {
            printf("Error: malloc(%ld) failed. Out of memory?\n", n);
            exit(EXIT_FAILURE);
        }
    return p;
}

// create the file system
fileSys *initFiles(){
    fileSys *files=safeMalloc(sizeof(fileSys));
    file *root = safeMalloc(sizeof(file));
    strcpy(root->name,"/");
    root->isDirectory = 1;
    root->parent = root;
    root->referenceCount = 1;
    root->numChild = 0;
    for(int i = 0; i < 50; i++){
        root->content.children[i] = NULL;
    }
    files->root = root;
    files->active = root;
    return files;
}

void changeActiveDirectory(fileSys **files, file *newDir) {
    printf("%s\n", (*files)->active->name);
    (*files)->active = newDir;  // This modifies the actual pointer inside fileSys
    printf("%s\n", (*files)->active->name);
}

//function to create files for the trie
file *createFile(char *name, int isDirectory, file *parent, char *data){
    file *newnode = (file *)safeMalloc(sizeof(file));
    strcpy(newnode->name, name);
    newnode->isDirectory = isDirectory;
    newnode->parent = parent;

    if (isDirectory) {
        for (int i = 0; i < 50; i++) {
            newnode->content.children[i] = NULL;
        }
    } else {
        newnode->content.data = data; // Regular file starts with no content
    }
    return newnode;
}

file *searchFile(file *root, char *path) {
    if (root == NULL) {
        return NULL;
    }
    if(path == NULL){
        return root;
    }

    // If searching for the root directory
    if (strcmp(path, "/") == 0) {
        return root;
    }

    char pathCopy[256];  // No need for malloc, use a fixed-size array
    strncpy(pathCopy, path, sizeof(path) - 1);  // Copy the commandLine to pathCopy

    // Ensure null-termination just in case strncpy doesn't null-terminate
    pathCopy[sizeof(pathCopy) - 1] = '\0';
    char *token = strtok(pathCopy, "/"); // Tokenize by "/"
    file *current = root;
    
    while (token != NULL) {
        if (!current->isDirectory) { // If it's a regular file, we can't go deeper
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
            return NULL;
        }

        token = strtok(NULL, "/");
    }
    return current; // Return the found file/directory
}

//function that inserts an file to the directory using path
void insertFile(file *root, char *path, int isDirectory, char *data) {
    char *name = strrchr(path, '/');
    char parentPath[256]; // Temporary buffer for the parent path

    if (name != NULL) {
        // Temporarily terminate the string to isolate the parent path
        *name = '\0'; // This modifies the original path
        name++; // Move past the '/'
        strncpy(parentPath, path, sizeof(parentPath) - 1); // Copy the parent path
        parentPath[sizeof(parentPath) - 1] = '\0'; // Ensure null-termination
    } else {
        name = path; // No slashes, it is the name itself
        strcpy(parentPath, ""); // No parent path
    }

    // Search for the parent directory using the modified path
    file *parent = searchFile(root, parentPath);

    if (parent == NULL || !parent->isDirectory) return;

    // Create the new file or directory
    file *newFile = createFile(name, isDirectory, parent, data);

    // Insert into parent's children array
    for (int i = 0; i < 50; i++) {
        if (parent->content.children[i] == NULL) {
            parent->content.children[i] = newFile;
            return; // Success
        }
    }
}

//function to free the trie function
void freeFile(file *root) {
    if(root == NULL) {
        return;
    }
    if(root->isDirectory){
        for (int i = 0; i < 50; i++) {
            if (root->content.children[i] != NULL) {
                freeFile(root->content.children[i]); // Recursively free child
            } else {
                break; // No more children to process
            }
        }
    }
    else {
        free(root->content.data);
    }
    free(root);
}

void removeFile(file *root, char *path){
    file *target = searchFile(root, path);

    if(target == NULL) return;

    // Remove the target file from the parent's children list
    if(target->parent != NULL){
        for (int i = 0; i < 50; i++) {
            if ((target->parent)->content.children[i] == target) {
                (target->parent)->content.children[i] = NULL;  // Remove it from the array
                break;
            }
        }
    }

    freeFile(target);
}

void printFileSystem(file *root, int depth) {
    if (root == NULL) {
        return;  // Base case: nothing to print
    }

    // Indentation based on depth
    for (int i = 0; i < depth; i++) {
        printf("  ");  // Two spaces for each level of depth
    }

    if (root->isDirectory) {
        printf("[Directory] %s\n", root->name);  // Print the directory name
        // Recursively print the contents of the directory
        for (int i = 0; i < 50; i++) {
            if (root->content.children[i] != NULL) {
                printFileSystem(root->content.children[i], depth + 1);  // Recursive call for subdirectories or files
            } else {
                break;  // No more children to process
            }
        }
    } else {
        printf("[File] %s: %s\n", root->name, root->content.data ? root->content.data : "No data");  // Print file name and content
    }
}