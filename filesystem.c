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

        for (int i = 0; i < sizeof(name); i++) {
            if (name[i] == '\0') {
                printf("name has a null character at position %d\n", i);
                break;
            }
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
    char *ptr1 = input; // Pointer to the start of the input
    char *ptr2; // Pointer to the delimiter
    char cont[256];

    // Loop to find and process each segment separated by ""
    while ((ptr2 = strchr(ptr1, '\"')) != NULL) {
        // Calculate the length of the substring
        size_t length = ptr2 - ptr1;

        // Copy the substring into the cont
        memcpy(cont, ptr1, length);
        cont[length] = '\0'; // Null-terminate the string

        // Move the pointer past the delimiter
        ptr1 = ptr2 + 1;
    }
    ptr1++;
    char *token = strtok(ptr1, " ");
    int type = strlen(token);
    token = strtok(NULL, " ");
    if(token == NULL){
        return;
    }

    // Handle nested directory creation (path contains "/")
    file *originalActive = (*files)->active;
    
    char *name = strrchr(token, '/');
    char parentPath[256]; // Temporary buffer for the parent path

    if (name != NULL) {
        // Temporarily terminate the string to isolate the parent path
        *name = '\0'; // This modifies the original path
        name++; // Move past the '/'
        strncpy(parentPath, token, sizeof(parentPath) - 1); // Copy the parent path
        parentPath[sizeof(parentPath) - 1] = '\0'; // Ensure null-termination
    } else {
        name = token; // No slashes, it is the name itself
        strcpy(parentPath, ""); // No parent path
    }

    file *srcParent = searchFile((*files)->root, parentPath);

    int ok = -1;
    for(int i=0; i < srcParent->numChild; i++){
        if(strcmp(srcParent->content.children[i]->name, name) == 0){
            ok = i;
            break;
        }
    }
    if(ok != -1){
        file *target = srcParent->content.children[ok];
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
        file *newFile = createFile(name, 0, srcParent, cont);

        for (int i = 0; i < 50; i++) {
            if (srcParent->content.children[i] == NULL) {
                srcParent->content.children[i] = newFile;
                srcParent->numChild++;
                break; // Success
            }
        }
    }
    (*files)->active = originalActive;
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
    char destDirPath[256];
    char destFilePath[256];
    strcpy(sourcePath, storePaths[0]);
    strcpy(destFilePath, storePaths[1]);
    char *destName = strrchr(storePaths[1], '/');

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
    file *srcParent = sourceFile->parent;
    
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
        file* destDir = searchFile((*files)->root, destDirPath);
        strcpy(sourceFile->name, destName);
        
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
        // Move the pointer past "-r" and any spaces
        path += 3; // Skip "-p"
        recursive = 1;
    }
    char *path1 = strtok(path, " ");
    if(path1 == NULL){
        return;
    }
    char *path2 = strtok(NULL, " ");
    if(path2 == NULL){
        return;
    }
    char *name = strrchr(path2, '/');
    char parentPath[256];
    if (name != NULL) {
        // Temporarily terminate the string to isolate the parent path
        *name = '\0'; // This modifies the original path
        name++; // Move past the '/'
        strncpy(parentPath, path2, sizeof(parentPath) - 1); // Copy the parent path
        parentPath[sizeof(parentPath) - 1] = '\0'; // Ensure null-termination
    } else {
        name = path2; // No slashes, it is the name itself
        strcpy(parentPath, ""); // No parent path
    }
    file *source = searchFile((*files)->root, path1);
    file *parent = searchFile((*files)->root, parentPath);
    if(source == NULL || parent == NULL){
        return;
    }
    if(recursive == 0){
        int ok = -1;
        for(int i = 0; i < parent->numChild; i++){
            if(strcmp(parent->content.children[i]->name, name) == 0){
                strcpy(parent->content.children[i]->content.data, source->content.data);
                // strcpy(parent->content.children[i]->name, source->name);
                ok = 1;
                break;
            }
        }
        if(ok == -1){
            file *newFile = createFile(name, 0, parent, NULL);
            strcpy(newFile->content.data, source->content.data);
            // strcpy(newFile->name, source->name);
            newFile->numChild = source->numChild;
        }
    }
    else{
        file *newDirectory = createFile(name, 1, parent, NULL);
        copyDirectory(source, newDirectory);
    } 
}

void rm(fileSys **files, char *path){
    if (strncmp(path, "-r", 2) == 0) {
        // Move the pointer past "-r" and any spaces
        path += 3; // Skip "-r"
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
    char **storePaths = makeCharArray2D(256, 256);
    int pathTotal = 0;

    // Tokenize the string by spaces to get each file path
    char *token = strtok(input, " "); // Get the first token

    while (token != NULL) {
        strcpy(storePaths[pathTotal], token);
        pathTotal++; // Increment the path counter

        // Get the next file path
        token = strtok(NULL, " "); // Get the next token
    }

    file *sFile =searchFile((*fs)->root, storePaths[0]);
    if(sFile == NULL || sFile->isDirectory == 1){
        return;
    }
    char destPath[256];
    strcpy(destPath, storePaths[1]);
    char *dName = strrchr(destPath, '/');
    char path[200];
    if(dName!=NULL){
        *dName = '\0';
        dName++;
        strncpy(path, destPath, sizeof(path)-1);
        path[sizeof(path)-1] = '\0';
    }
    else{
        dName = destPath;
        strcpy(path, (*fs)->active->name);
    }
    printf("%s\n", path);
    printf("%s\n", destPath);
    printf("%s\n\n", dName);
    file *parentDirectory = searchFile((*fs)->root, path);
    
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

    destroyArray2D(storePaths, 256);
}