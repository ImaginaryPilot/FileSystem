#include "filesystem.h"
#include "createFile.h"

void cd(fileSys **files, char *path){
    if (path == NULL || strcmp(path, "/") == 0) {
        // If the path is NULL or "/", go to the root directory
        changeActiveDirectory(files, (*files)->root);
        return;
    }

    if (strcmp(path, "..") == 0) {
        // If the user enters "..", go to the parent directory
        if ((*files)->active->parent != NULL) {
            changeActiveDirectory(files, (*files)->active->parent);
        }
        return;
    }

    // Try to find the directory by using searchFile
    file *newDir = searchFile((*files)->active, path);
    if (newDir != NULL && newDir->isDirectory) {
        // If the directory exists and it's a directory, change the currentDir
        changeActiveDirectory(files, newDir);
    }
}

void ls(file *active){
    if(active->isDirectory == 0  || active->numChild == 0) {
        return;
    }
    for(int i = 0; i < active->numChild; i++){
        for(int j = i+1; j <= active->numChild; j++){
            if(active->content.children[j] == NULL) continue;
            if(strcmp(active->content.children[i]->name, active->content.children[j]->name)>0){
                file *temp = active->content.children[i];
                active->content.children[i] = active->content.children[j];
                active->content.children[j] = temp;
            }
        }
    }
    for(int i = 0; i< active->numChild; i++){
        printf("%s\n", active->content.children[i]->name);
    }
}

void cat(fileSys **files, char *path){
    file *newDir = searchFile((*files)->active, path);
    printf("%s\n", newDir->content.data);
}

void find(file *root, char *path){
    if (root == NULL) return;

    printf("%s\n", path);
    
    if(root->isDirectory){
        // Sort the children in ASCII order
        qsort(root->content.children, root->numChild, sizeof(file *), compareFiles);

        for(int i = 0; i < root->numChild; i++){
            if(root->content.children[i] == NULL) continue;

            char newPath[256];
            snprintf(newPath, sizeof(newPath), "%s/%s", path, root->content.children[i]->name);

            find(root->content.children[i], newPath);
        }
    }
}

void touch(fileSys **files, char *path) {
    // Handle nested directory creation (path contains "/")
    file *originalActive = (*files)->active;
    
    insertFile((*files)->root, path, 0, NULL);

    // Reset the active directory back to the original
    (*files)->active = originalActive;
}

void echo(fileSys **files, char *path){
    printf("%s\n", path);

    char pathCopy[256];  // No need for malloc, use a fixed-size array
    strncpy(pathCopy, path, sizeof(path) - 1);  // Copy the commandLine to pathCopy
}

void mkdir(fileSys **files, char *path){
    if (strncmp(path, "-p", 2) == 0) {
        // Move the pointer past "-p" and any spaces
        path += 3; // Skip "-p"
    }
    printf("%s\n", path);

    char **storePaths = makeCharArray2D(256, 256);
    int pathTotal = 0;

    // Tokenize the string by spaces to get each file path
    char *token = strtok(path, " "); // Get the first token

    while (token != NULL) {
        strcpy(storePaths[pathTotal], token);
        pathTotal++; // Increment the path counter

        // Get the next file path
        token = strtok(NULL, " "); // Get the next token
    }
    
    pathTotal--;
    while (pathTotal >= 0) {
        char *pathPart = storePaths[pathTotal];
        printf("pathpart: %s\n", pathPart);

        // Process each path as a separate mkdir
        file *originalActive = (*files)->active;
        file *current = (*files)->active;

        char *token = strtok(pathPart, "/");  // Tokenize the path by "/"
        while (token != NULL) {
            int found = 0;
            // Check if the directory exists, if yes, move into it
            for (int i = 0; i < current->numChild; i++) {
                if (current->content.children[i] != NULL && strcmp(current->content.children[i]->name, token) == 0) {
                    current = current->content.children[i];
                    found = 1;
                    break;
                }
            }

            // If directory does not exist, create it
            if (!found) {
                file *newFile = createFile(token, 1, current, NULL);

                // Insert into parent's children array
                for (int i = 0; i < 50; i++) {
                    if (current->content.children[i] == NULL) {
                        current->content.children[i] = newFile;
                        current->numChild++;
                        current = newFile;  // Move to the newly created directory
                        break;
                    }
                }
            }

            // Get the next directory in the path
            token = strtok(NULL, "/");
        }

        // Reset the active directory back to the original
        (*files)->active = originalActive;

        pathTotal--;
    }

    destroyArray2D(storePaths, 256);
}

void mv(){
    printf("mv");
}

void cp(){
    printf("cp");
}

void rm(){
    printf("rm");
}

void ln(){
    printf("ln");
}