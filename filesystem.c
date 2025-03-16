#include "filesystem.h"
#include "createFile.h"

void cd(fileSys **files, char *path){
    if (path == NULL || strcmp(path, "/") == 0) {
        // If the path is NULL or "/", go to the root directory
        changeActiveDirectory(files, (*files)->root);
        return;
    }

    if (strcmp(path, ".") == 0) return;

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

void ls(fileSys **files, char* path){
    file* active;
    if(path == NULL || strcmp(path, ".") == 0){
        active = (*files)->active;
    }
    else {
        active = searchFile((*files)->root, path);
    }
    
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
    if(path != NULL){
        file *newDir = searchFile((*files)->root, path);
        if(newDir == NULL || newDir->isDirectory || newDir->content.data[0] == '\0') return;
        printf("%s\n", newDir->content.data);
    }
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

    while(pathTotal >= 0){
        char *pathPart = storePaths[pathTotal];

        // Handle nested directory creation (path contains "/")
        file *originalActive = (*files)->active;
        
        char *name = strrchr(pathPart, '/');
        char parentPath[256]; // Temporary buffer for the parent path

        if (name != NULL) {
            // Temporarily terminate the string to isolate the parent path
            *name = '\0'; // This modifies the original path
            name++; // Move past the '/'
            strncpy(parentPath, pathPart, sizeof(parentPath) - 1); // Copy the parent path
            parentPath[sizeof(parentPath) - 1] = '\0'; // Ensure null-termination
        } else {
            name = pathPart; // No slashes, it is the name itself
            strcpy(parentPath, ""); // No parent path
        }

        // Search for the parent directory using the modified path
        file *parent = searchFile((*files)->root, parentPath);

        if (parent == NULL || !parent->isDirectory) continue;

        for (int i = 0; i < 50; i++) {
            if(parent->content.children[i] != NULL && strcmp(parent->content.children[i]->name, name) == 0) return;
        }

        // Create the new file or directory
        file *newFile = createFile(name, 0, parent, NULL);

        // Insert into parent's children array
        for (int i = 0; i < 50; i++) {
            if (parent->content.children[i] == NULL) {
                parent->content.children[i] = newFile;
                parent->numChild++;
                break; // Success
            }
        }

        // Reset the active directory back to the original
        (*files)->active = originalActive;

        pathTotal--;
    }
    
    destroyArray2D(storePaths, 256);
}

void echo(fileSys **files, char *input){
    //get string that needs to be put in file
    char *token = strtok(input, "\"");

    //if no string return
    if(token == NULL){
        return;
    }

    //keep string in variable
    char *cont = token;
    token = strtok(NULL, " ");
    //check which type of echo we have
    if(token == NULL){
        return;
    }
    int type = strlen(token);
    token = strtok(NULL, " ");
    if(token == NULL){
        return;
    }
    int ok = -1;
    for(int i=0; i < (*files)->active->numChild; i++){
        if(strcmp((*files)->active->content.children[i]->name, token) == 0){
            ok = i;
            break;
        }
    }
    if(ok != -1){
        file *target = (*files)->active->content.children[ok];
        if(type == 1){
            strcpy(target->content.data, cont);
        }
        else{
            if (target->content.data[0] == '\0') {
                strcpy(target->content.data, cont);
            }
            else{
                strcat(target->content.data, cont);
            }
        }
    }
    else{
        char path[256];
        strcpy(path, token);

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

        file *parent = searchFile((*files)->root, parentPath);
        file *newFile = createFile(name, 0, parent, cont);

        for (int i = 0; i < 50; i++) {
            if (parent->content.children[i] == NULL) {
                parent->content.children[i] = newFile;
                parent->numChild++;
                break; // Success
            }
        }
    }
}

void mkdir(fileSys **files, char *path){
    if (strncmp(path, "-p", 2) == 0) {
        // Move the pointer past "-p" and any spaces
        path += 3; // Skip "-p"
    }

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

void mv(fileSys **files, char *path){
    char **storePaths = makeCharArray2D(2, 256);
    int pathIdx = 0;

    // Tokenize the string by spaces to get each file path
    char *token = strtok(path, " "); // Get the first token

    while (token != NULL) {
        strcpy(storePaths[pathIdx], token);
        pathIdx++; // Increment the path counter

        // Get the next file path
        token = strtok(NULL, " "); // Get the next token
    }
    
    pathIdx = 0;

    // Handle nested directory creation (path contains "/")
    file *originalActive = (*files)->active;

    char sourcePath[256];
    char srcParentPath[256];
    char destDirPath[256];
    char destFilePath[256];
    strcpy(sourcePath, storePaths[0]);
    strcpy(destFilePath, storePaths[1]);

    char *srcName = strrchr(storePaths[0], '/');
    char *destName = strrchr(storePaths[1], '/');

    if (srcName != NULL) {
        // Temporarily terminate the string to isolate the parent path
        *srcName = '\0'; // This modifies the original path
        srcName++; // Move past the '/'
        strncpy(srcParentPath, storePaths[0], sizeof(srcParentPath) - 1); // Copy the parent path
        srcParentPath[sizeof(srcParentPath) - 1] = '\0'; // Ensure null-termination
    } else {
        srcName = storePaths[0]; // No slashes, it is the name itself
        strcpy(srcParentPath, ""); // No parent path
    }

    if (destName != NULL) {
        // Temporarily terminate the string to isolate the parent path
        *destName = '\0'; // This modifies the original path
        destName++; // Move past the '/'
        strncpy(destDirPath, storePaths[1], sizeof(destDirPath) - 1); // Copy the parent path
        destDirPath[sizeof(destDirPath) - 1] = '\0'; // Ensure null-termination
    } else {
        destName = storePaths[1]; // No slashes, it is the name itself
        strcpy(destDirPath, ""); // No parent path
    }

    // Search for the parent directory using the modified path
    file *sourceFile = searchFile((*files)->root, storePaths[0]);
    file *srcParent = searchFile((*files)->root, srcParentPath);
    
    if(!sourceFile->isDirectory){
        file *destDir = searchFile((*files)->root, destDirPath);
        file *destFile = searchFile((*files)->root, destFilePath);
        
        int exists = (destFile == NULL) ? 0 : 1;
        
        if(!exists){
            for (int i = 0; i < 50; i++) {
                if (destDir->content.children[i] == NULL) {
                    strcpy(sourceFile->name, destName);
                    destDir->content.children[i] = sourceFile;
                    sourceFile->parent = destDir;
                    destDir->numChild++;
                    break;
                }
            }
        }
        else {
            for(int i = 0; i < destDir->numChild; i++){
                if (strcmp(destDir->content.children[i]->name, destFile->name) == 0) {
                    strcpy(sourceFile->name, destName);
                    file *toRemove = destDir->content.children[i];
                    destDir->content.children[i] = sourceFile;
                    removeFile(toRemove, destFilePath);
                    sourceFile->parent = destDir;
                    break;
                }
            }
        }
        
        int foundIdx = -1;
        for(int i = 0; i < srcParent->numChild; i++){
            if(srcParent->content.children[i] == sourceFile){
                foundIdx = i;
                break;
            }
        }
        if(foundIdx != -1){
            // Shift all elements left to maintain order
            for (int i = foundIdx; i < srcParent->numChild - 1; i++) {
                srcParent->content.children[i] = srcParent->content.children[i + 1];
            }
            srcParent->content.children[srcParent->numChild - 1] = NULL;  // Clear last slot
            srcParent->numChild--;
        }
    }
    else {
        printf("testcases:\n");
        printf("%s\n", destDirPath);
        printf("%s\n", destFilePath);
        printf("%s\n", destName);
        printf("%s\n", srcName);
        printf("%s\n", sourceFile->name);
        printf("%s\n", srcParent->name);

        file *destDir = searchFile((*files)->root, destFilePath);
        if(destDir == NULL){
            file *destDirParent = searchFile((*files)->root, destDirPath);
            destDir = createFile(destName, 1, destDirParent, NULL);
            printf("%s\n", destDir->name);
            printf("%s\n", destDirParent->name);

            // Insert into parent's children array
            for (int i = 0; i < 50; i++) {
                if (destDirParent->content.children[i] == NULL) {
                    destDirParent->content.children[i] = destDir;
                    destDirParent->numChild++;
                    break;
                }
            }
        }
        
        for (int i = 0; i < 50; i++) {
            if (destDir->content.children[i] == NULL) {
                destDir->content.children[i] = sourceFile;
                sourceFile->parent = destDir;
                destDir->numChild++;
                break;
            }
        }

        int foundIdx = -1;
        for(int i = 0; i < srcParent->numChild; i++){
            if(srcParent->content.children[i] == sourceFile){
                foundIdx = i;
                break;
            }
        }
        if(foundIdx != -1){
            // Shift all elements left to maintain order
            for (int i = foundIdx; i < srcParent->numChild - 1; i++) {
                srcParent->content.children[i] = srcParent->content.children[i + 1];
            }
            srcParent->content.children[srcParent->numChild - 1] = NULL;  // Clear last slot
            srcParent->numChild--;
        }
    }
    
    // Reset the active directory back to the original
    (*files)->active = originalActive;

    destroyArray2D(storePaths, 2);
}

void cp(fileSys **files, char *path){
    int recursive = 0;
    if (strncmp(path, "-r", 2) == 0) {
        // Move the pointer past "-p" and any spaces
        path += 3; // Skip "-p"
        recursive = 1;
    }
    char *path1 = strtok(path, " ");
    if(path1 == NULL){
        return;
    }
    char *path2 = strtok(NULL, " ");
    if(recursive == 0){
        
    }
    printf("%s\n", path1);
    printf("%s\n", path2);
}

void rm(fileSys **files, char *path){
    if (strncmp(path, "-r", 2) == 0) {
        // Move the pointer past "-p" and any spaces
        path += 3; // Skip "-p"
    }

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

    while(pathTotal >= 0){
        char *pathPart = storePaths[pathTotal];

        // Handle nested directory creation (path contains "/")
        file *originalActive = (*files)->active;

        // Search for the parent directory using the modified path
        file *target = searchFile((*files)->root, pathPart);

        if (target == NULL) {
            pathTotal--;
            continue;  // Skip this entry if the file is not found
        }

        removeFile(target, pathPart);

        // Reset the active directory back to the original
        (*files)->active = originalActive;

        pathTotal--;
    }
    
    destroyArray2D(storePaths, 256);
}

void ln(fileSys **fs, char *input){
    char *source = strtok(input, " ");
    char *destination = strtok(input, " ");
    if(source == NULL || destination == NULL){
        return;
    }
    file *sFile =searchFile((*fs)->root, source);
    if(sFile == NULL || sFile->isDirectory == 1){
        return;
    }
    char *dName = strrchr(destination, '/');
    char path[200];
    if(dName!=NULL){
        *dName = '\0';
        dName++;
        strncpy(path, destination, sizeof(path)-1);
        path[sizeof(path)-1] = '\0';
    }
    else{
        dName = destination;
        strcpy(path, (*fs)->active->name);
    }
    file *parentDirectory = searchFile((*fs)->root, path);
    if(parentDirectory == NULL || parentDirectory->isDirectory == 0){
        return;
    }
    for(int i=0; i<=parentDirectory->numChild; i++){
        if(parentDirectory->content.children[i]->name == dName){
            return;
        }
    }
    file *newHL = createFile(dName, 0, parentDirectory, NULL);
    newHL->referenceCount = sFile->referenceCount + 1;
    newHL->numChild = 0;
    strcpy(newHL->content.data, sFile->content.data);
    for(int i=0; i<50; i++){
        if(parentDirectory->content.children[i] == NULL){
            parentDirectory->content.children[i] = newHL;
            parentDirectory->numChild++;
            sFile->referenceCount++;
            return;
        }
    }
}