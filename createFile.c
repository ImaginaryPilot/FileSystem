#include "createFile.h"

void *safeMalloc(size_t n) {
    void *p = malloc(n);
        if (p == NULL) {
            printf("Error: malloc(%ld) failed. Out of memory?\n", n);
            exit(EXIT_FAILURE);
        }
    return p;
}

// Making 2D array memory for type int
char **makeCharArray2D(int height, int width) {
    char **arr = (char **)safeMalloc(height*sizeof(char *));
        for (int row=0; row < height; row++) {
            arr[row] = (char *)safeMalloc(width*sizeof(char));
        }
    return arr;
}

// free 2D array
void destroyArray2D(char **arr, int height) {
   for (int row=0; row < height; row++) {
      free(arr[row]);
   }
   free(arr);
}

// create the file system
fileSys *initFiles(){
    fileSys *files=safeMalloc(sizeof(fileSys));
    file *root = safeMalloc(sizeof(file));
    strcpy(root->name,"/");
    root->isDirectory = 1;
    root->parent = root;
    root->numChild = 0;
    root->inode = NULL;
    for(int i = 0; i < 256; i++){
        root->content.children[i] = NULL;
    }
    files->root = root;
    files->active = root;
    return files;
}

void changeActiveDirectory(fileSys **files, file *newDir) {
    // This modifies the actual pointer inside fileSys
    (*files)->active = newDir;  
}

// Comparison function for qsort to sort in ASCII order
int compareFiles(const void *a, const void *b) {
    // Cast the void pointers to file pointers
    file *fileA = *(file **)a;
    file *fileB = *(file **)b;
    // Compare the names of the files/directories in ASCII order
    return strcmp(fileA->name, fileB->name);
}

//function to create files for the trie
file *createFile(char *name, int isDirectory, file *parent, char *data, inode *inodeRef){
    file *newnode = (file *)safeMalloc(sizeof(file));
    inode *newInode = (inode *)safeMalloc(sizeof(inode));
    
    // Initialize all fields of the new node
    // Clear out the entire struct to avoid uninitialized fields
    memset(newnode, 0, sizeof(file)); 
    memset(newInode, 0, sizeof(inode)); 
    strcpy(newnode->name, name);
    newnode->name[sizeof(newnode->name) - 1] = '\0';
    newnode->isDirectory = isDirectory;
    newnode->parent = parent;
    if(inodeRef == NULL){
        newnode->inode = newInode;
        if (data != NULL) {
            // Ensure the string does not overflow the data buffer
            strncpy(newnode->inode->data, data, sizeof(newnode->inode->data) - 1);
            newnode->inode->data[sizeof(newnode->inode->data) - 1] = '\0';
        } else {
            // Make sure the data is an empty string if no data is provided
            newnode->inode->data[0] = '\0';  
        }
        for (int i = 0; i < 256; i++) {
            newnode->inode->links[i] = NULL;
        }
        newnode->inode->links[0] = newnode;
        newnode->inode->referenceCount++;
    }
    else{
        free(newInode);
        newnode->inode = inodeRef;
        for(int i=0; i<256; i++){
            if(newnode->inode->links[i] == NULL){
                newnode->inode->links[i] = newnode;
                newnode->inode->referenceCount++;
                break;
            }
        }
    }
    // Initialize child count for directories
    newnode->numChild = 0;            
    if (isDirectory) {
        for (int i = 0; i < 256; i++) {
            newnode->content.children[i] = NULL;
        }
    } else {
        // If it's a regular file, copy the data string if data is not NULL
        if (data != NULL) {
            // Ensure the string does not overflow the data buffer
            strncpy(newnode->content.data, data, sizeof(newnode->content.data) - 1);
            // Null-terminate to prevent overflow
            newnode->content.data[sizeof(newnode->content.data) - 1] = '\0';  
        } else {
            // Make sure the data is an empty string if no data is provided
            newnode->content.data[0] = '\0';  
        }
    }
    return newnode;
}

file *searchFile(fileSys **files, file *root, char *path) {
    if (root == NULL) return NULL;
    if (path == NULL) return root;
    // If searching for the root directory
    if (strcmp(path, "/") == 0) return root;
    // No need for malloc, use a fixed-size array
    char pathCopy[256];  
    // Copy the commandLine to pathCopy
    strncpy(pathCopy, path, sizeof(pathCopy) - 1);  
    int isAbsolutePath = (path[0] == '/');
    // Ensure null-termination just in case strncpy doesn't null-terminate
    pathCopy[sizeof(pathCopy) - 1] = '\0';
    // Tokenize by "/"
    char *token = strtok(pathCopy, "/"); 
    file *current = isAbsolutePath ? (*files)->root : root;
    
    while (token != NULL) {
        // If it's a regular file, we can't go deeper
        if (!current->isDirectory) { 
            return NULL;
        }
        if (strcmp(token, ".") == 0) {
            token = strtok(NULL, "/");
            continue;
        }
        if (strcmp(token, "..") == 0) {
            current = current->parent;
            token = strtok(NULL, "/");
            continue;
        }
        int found = 0;
        for (int i = 0; i < 256; i++) {
            if (current->content.children[i] != NULL && strcmp(current->content.children[i]->name, token) == 0) {
                current = current->content.children[i];
                found = 1;
                break;
            }
        }
        // If no match found, return NULL
        if (!found) { 
            return NULL;
        }

        token = strtok(NULL, "/");
    }
    // Return the found file/directory
    return current; 
}

//function to free the trie function
void freeFile(file *root) {
    if(root == NULL) {
        return;
    }
    if(root->isDirectory){
        for (int i = 0; i < 256; i++) {
            if (root->content.children[i] != NULL) {
                // Recursively free child
                freeFile(root->content.children[i]); 
                // Ensure pointers are cleared
                root->content.children[i] = NULL;  
            }
        }
    }
    // Decrement the reference count of the inode
    if (root->inode != NULL) {
        int foundIdx = -1;
        for (int i = 0; i < 256; i++) {
            if (root->inode->links[i] == root) {
                foundIdx = i;
                break;
            }
        }

        if (foundIdx != -1) {
            // Shift all elements left to maintain order
            for (int i = foundIdx; i < root->inode->referenceCount; i++) {
                root->inode->links[i] = root->inode->links[i + 1];
            }
            // Clear last slot
            root->inode->links[root->inode->referenceCount - 1] = NULL;  
            root->inode->referenceCount--;
        }
        // If the reference count is now 0, free the inode
        if (root->inode->referenceCount == 0) {
            free(root->inode);
        }
    }

    free(root);
}

//function that copies directories recursively
void copyDirectory(file *source, file *destination){
    //go through the children of the copied directory and copy them in the new one
    for(int i=0; i < source->numChild; i++){
        file *child = createFile(source->content.children[i]->name, source->content.children[i]->isDirectory, destination, source->content.children[i]->content.data, NULL);
        //insert new child in children array
        for (int i = 0; i < 50; i++) {
            if (destination->content.children[i] == NULL) {
                destination->content.children[i] = child;
                destination->numChild++;
                break; 
            }
        }
        //if the children is also a directory, copy the entire directory recursively
        if(child->isDirectory == 1){
            copyDirectory(source->content.children[i], child);
        }
    }
}

//function that removes files
void removeFile(file *root, char *path){
    file *target = root;
    if(target == NULL) return;
    file *parent = target->parent;
    // Remove the target file from the parent's children list
    if(parent != NULL){
        int foundIdx = -1;
        for (int i = 0; i < 256; i++) {
            if (parent->content.children[i] == target) {
                foundIdx = i;
                break;
            }
        }
        if (foundIdx != -1) {
            // Shift all elements left to maintain order
            for (int i = foundIdx; i < parent->numChild; i++) {
                parent->content.children[i] = parent->content.children[i + 1];
            }
            // Clear last slot
            parent->content.children[parent->numChild - 1] = NULL;  
            parent->numChild--;
        }
    }
    freeFile(target);
}