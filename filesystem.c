#include "filesystem.h"
#include "createFile.h"

//cd function
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
    file *newDir = searchFile(files, (*files)->active, path);
    if (newDir != NULL && newDir->isDirectory) {
        // If the directory exists and it's a directory, change the currentDir
        changeActiveDirectory(files, newDir);
    }
}

//ls function
void ls(fileSys **files, char* path){
    file* active;
    if(path == NULL || strcmp(path, ".") == 0){
        //set active directory
        active = (*files)->active;
    }
    else {
        //search for specified file in directory
        active = searchFile(files, (*files)->root, path);
    }
    if(active->isDirectory == 0  || active->numChild == 0) {
        return;
    }
    //sort the directory alphabetically with bubblesort
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
    //print all children
    for(int i = 0; i< active->numChild; i++){
        printf("%s\n", active->content.children[i]->name);
    }
}

//cat function
void cat(fileSys **files, char *path){
    //ensure valid path is provided
    if(path != NULL){
        //search for file needed
        file *newDir = searchFile(files, (*files)->active, path);
        if(newDir == NULL || newDir->isDirectory || newDir->content.data[0] == '\0') return;
        //print the content
        printf("%s\n", newDir->content.data);
    }
}

//find function
void find(file *root, char *path){
    //if the root is null, 
    if (root == NULL) return;
    printf("%s\n", path);
    if(root->isDirectory){
        // Sort the children in ASCII order
        qsort(root->content.children, root->numChild, sizeof(file *), compareFiles);
        for(int i = 0; i < root->numChild; i++){
            //skip null children
            if(root->content.children[i] == NULL) continue;
            //store new path
            char newPath[1024];
            snprintf(newPath, sizeof(newPath), "%s/%s", path, root->content.children[i]->name);
            //recursively call find 
            find(root->content.children[i], newPath);
        }
    }
}

//touch function
void touch(fileSys **files, char *path) {
    char **storePaths = makeCharArray2D(256, 1024);
    int pathTotal = 0;
    // Tokenize the string by spaces to get each file path
    char *token = strtok(path, " "); 
    while (token != NULL) {
        strcpy(storePaths[pathTotal], token);
        // Increment the path counter
        pathTotal++; 
        // Get the next file path
        token = strtok(NULL, " "); 
    }
    pathTotal--;
    while(pathTotal >= 0){
        char *pathPart = storePaths[pathTotal];
        // Handle nested directory creation (path contains "/")
        file *originalActive = (*files)->active;
        char *name = strrchr(pathPart, '/');
        // Temporary buffer for the parent path
        char parentPath[1024]; 
        if (name != NULL) {
            // Temporarily terminate the string to isolate the parent path
            // This modifies the original path
            *name = '\0'; 
            // Move past the '/'
            name++; 
            // Copy the parent path
            strncpy(parentPath, pathPart, sizeof(parentPath) - 1); 
             // Ensure null-termination
            parentPath[sizeof(parentPath) - 1] = '\0';
        } else {
            // No slashes, it is the name itself
            name = pathPart; 
            // No parent path
            strcpy(parentPath, ""); 
        }
        // Search for the parent directory using the modified path
        file *parent = searchFile(files, (*files)->root, parentPath);
        if (parent == NULL || !parent->isDirectory) continue;
        for (int i = 0; i < 256; i++) {
            if(parent->content.children[i] != NULL && strcmp(parent->content.children[i]->name, name) == 0) return;
        }
        // Create the new file or directory
        file *newFile = createFile(name, 0, parent, NULL, NULL);
        // Insert into parent's children array
        for (int i = 0; i < 256; i++) {
            if (parent->content.children[i] == NULL) {
                parent->content.children[i] = newFile;
                parent->numChild++;
                break; 
                // Success
            }
        }
        // Reset the active directory back to the original
        (*files)->active = originalActive;
        pathTotal--;
    }
    destroyArray2D(storePaths, 256);
}

//echo function
void echo(fileSys **files, char *input){
    // Pointer to the start of the input
    char *ptr1 = input; 
    // Pointer to the delimiter
    char *ptr2; 
    char cont[1024];
    // Loop to find and process each segment separated by ""
    while ((ptr2 = strchr(ptr1, '\"')) != NULL) {
        // Calculate the length of the substring
        size_t length = ptr2 - ptr1;
        // Copy the substring into the cont
        memcpy(cont, ptr1, length);
        // Null-terminate the string
        cont[length] = '\0'; 
        // Move the pointer past the delimiter
        ptr1 = ptr2 + 1;
    }
    ptr1++;
    char *token = strtok(ptr1, " ");
    //checks if echo > or >>
    int type = strlen(token);
    token = strtok(NULL, " ");
    if(token == NULL){
        return;
    }
    // Handle nested directory creation (path contains "/")
    file *originalActive = (*files)->active;
    char *name = strrchr(token, '/');
    // Temporary buffer for the parent path
    char parentPath[1024]; 
    if (name != NULL) {
        // Temporarily terminate the string to isolate the parent path
        // This modifies the original path
        *name = '\0'; 
        // Move past the '/'
        name++; 
        // Copy the parent path
        strncpy(parentPath, token, sizeof(parentPath) - 1); 
        // Ensure null-termination
        parentPath[sizeof(parentPath) - 1] = '\0'; 
    } else {
        // No slashes, it is the name itself
        name = token; 
        // No parent path
        strcpy(parentPath, ""); 
    }
    file *srcParent = searchFile(files, (*files)->active, parentPath);
    //search if file with the same name already exists
    int ok = -1;
    for(int i=0; i < srcParent->numChild; i++){
        if(strcmp(srcParent->content.children[i]->name, name) == 0){
            ok = i;
            break;
        }
    }
    //if it exists
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
        for(int i = 0; i < target->inode->referenceCount; i++){
            if(target->inode->links[i] == NULL || target->inode->links[i] == target) continue;
            strcpy(target->inode->links[i]->content.data, target->content.data);
        }
    }
    //if it doesn't exist
    else{
        //create new file
        file *newFile = createFile(name, 0, srcParent, cont, NULL);
        //insert new child in children array
        for (int i = 0; i < 256; i++) {
            if (srcParent->content.children[i] == NULL) {
                srcParent->content.children[i] = newFile;
                srcParent->numChild++;
                break; 
                // Success
            }
        }
    }
    (*files)->active = originalActive;
}

//mkdir function
void mkdir(fileSys **files, char *path){
    if (strncmp(path, "-p", 2) == 0) {
        // Move the pointer past "-p" and any spaces
        // Skip "-p"
        path += 3; 
    }
    char **storePaths = makeCharArray2D(256, 1024);
    int pathTotal = 0;
    // Tokenize the string by spaces to get each file path
    // Get the first token
    char *token = strtok(path, " "); 
    while (token != NULL) {
        strcpy(storePaths[pathTotal], token);
        // Increment the path counter
        pathTotal++; 
        // Get the next file path
        // Get the next token
        token = strtok(NULL, " "); 
    }
    pathTotal--;
    while (pathTotal >= 0) {
        char *pathPart = storePaths[pathTotal];
        // Process each path as a separate mkdir
        file *originalActive = (*files)->active;
        file *current = (*files)->active;
        // Tokenize the path by "/"
        char *token = strtok(pathPart, "/");  
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
                file *newFile = createFile(token, 1, current, NULL, NULL);
                // Insert into parent's children array
                for (int i = 0; i < 256; i++) {
                    if (current->content.children[i] == NULL) {
                        current->content.children[i] = newFile;
                        current->numChild++;
                        // Move to the newly created directory
                        current = newFile;  
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

//mv function
void mv(fileSys **files, char *path){
    char **storePaths = makeCharArray2D(2, 1024);
    int pathIdx = 0;
    // Tokenize the string by spaces to get each file path
    // Get the first token
    char *token = strtok(path, " "); 
    while (token != NULL) {
        strcpy(storePaths[pathIdx], token);
        // Increment the path counter
        pathIdx++; 
        // Get the next file path
        // Get the next token
        token = strtok(NULL, " "); 
    }
    pathIdx = 0;
    // Handle nested directory creation (path contains "/")
    file *originalActive = (*files)->active;
    char sourcePath[1024];
    char destDirPath[1024];
    char destFilePath[1024];
    strcpy(sourcePath, storePaths[0]);
    strcpy(destFilePath, storePaths[1]);
    char *destName = strrchr(storePaths[1], '/');
    if (destName != NULL) {
        // Temporarily terminate the string to isolate the parent path
        // This modifies the original path
        *destName = '\0'; 
        // Move past the '/'
        destName++; 
        // Copy the parent path
        strncpy(destDirPath, storePaths[1], sizeof(destDirPath) - 1); 
        // Ensure null-termination
        destDirPath[sizeof(destDirPath) - 1] = '\0'; 
    } else {
        // No slashes, it is the name itself
        destName = storePaths[1]; 
        // No parent path
        strcpy(destDirPath, ""); 
    }
    // Search for the parent directory using the modified path
    file *sourceFile = searchFile(files, (*files)->root, storePaths[0]);
    file *srcParent = sourceFile->parent;
    //check if source file is directory
    if(!sourceFile->isDirectory){
        file *destDir = searchFile(files, (*files)->root, destDirPath);
        file *destFile = searchFile(files, (*files)->root, destFilePath);
        int exists = (destFile == NULL) ? 0 : 1;
        if(!exists){
            //check if theres an available index to put the file
            for (int i = 0; i < 256; i++) {
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
            //replace the already existing file
            for(int i = 0; i < destDir->numChild; i++){
                if (strcmp(destDir->content.children[i]->name, destFile->name) == 0) {
                    strcpy(sourceFile->name, destName);
                    //delete the file and put in the correct file
                    file *toRemove = destDir->content.children[i];
                    destDir->content.children[i] = sourceFile;
                    removeFile(toRemove, destFilePath);
                    sourceFile->parent = destDir;
                    break;
                }
            }
        }
        //check place of sourcefile in its parent
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
            // Clear last slot
            srcParent->content.children[srcParent->numChild - 1] = NULL;  
            srcParent->numChild--;
        }
    }
    else {
        //create the directory
        file* destDir = searchFile(files, (*files)->root, destDirPath);
        strcpy(sourceFile->name, destName);
        for (int i = 0; i < 256; i++) {
            if (destDir->content.children[i] == NULL) {
                destDir->content.children[i] = sourceFile;
                sourceFile->parent = destDir;
                destDir->numChild++;
                break;
            }
        }
        //check for existence of directory
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
            // Clear last slot
            srcParent->content.children[srcParent->numChild - 1] = NULL;  
            srcParent->numChild--;
        }
    }
    // Reset the active directory back to the original
    (*files)->active = originalActive;
    destroyArray2D(storePaths, 2);
}

//cp function
void cp(fileSys **files, char *path){
    int recursive = 0;
    if (strncmp(path, "-r", 2) == 0) {
        // Move the pointer past "-r" and any spaces
        // Skip "-r"
        path += 3; 
        recursive = 1;
    }
    //memorize first path
    char *path1 = strtok(path, " ");
    if(path1 == NULL){
        return;
    }
    //memorize second path
    char *path2 = strtok(NULL, " ");
    if(path2 == NULL){
        return;
    }
    char *name = strrchr(path2, '/');
    char parentPath[256];
    if (name != NULL) {
        // Temporarily terminate the string to isolate the parent path
        // This modifies the original path
        *name = '\0'; 
        // Move past the '/'
        name++; 
        // Copy the parent path
        strncpy(parentPath, path2, sizeof(parentPath) - 1); 
        // Ensure null-termination
        parentPath[sizeof(parentPath) - 1] = '\0'; 
    } else {
        // No slashes, it is the name itself
        name = path2; 
        // No parent path
        strcpy(parentPath, ""); 
    }
    file *source = searchFile(files, (*files)->active, path1);
    file *parent = searchFile(files, (*files)->active, parentPath);
    if(source == NULL || parent == NULL){
        return;
    }
    if(recursive == 0){
        //if the file already exists
        int ok = -1;
        for(int i = 0; i < parent->numChild; i++){
            if(strcmp(parent->content.children[i]->name, name) == 0){
                file *newFile = createFile(name, 0, parent, parent->content.children[i]->content.data, NULL);
                file *toRemove = parent->content.children[i];
                parent->content.children[i] = newFile;
                removeFile(toRemove, path2);
                ok = i;
                break;  
            }
        }
        //if the file is not found
        if(ok == -1){
            file *newFile = createFile(name, 0, parent, NULL, NULL);
            for (int i = 0; i < 50; i++) {
                if (parent->content.children[i] == NULL) {
                    parent->content.children[i] = newFile;
                    parent->numChild++;
                    break; 
                    // Success
                }
            }
            if(source->content.data[0] !='\0'){
                strcpy(newFile->content.data, source->content.data);
            }
            else {
                // Null-terminate to prevent overflow
                newFile->content.data[sizeof(newFile->content.data) - 1] = '\0';  
            } 
        }
    }
    //if copy directory
    else{
        file *newDirectory = createFile(name, 1, parent, NULL, NULL);
        for (int i = 0; i < 50; i++) {
            if (parent->content.children[i] == NULL) {
                parent->content.children[i] = newDirectory;
                parent->numChild++;
                break; 
            }
        }
        copyDirectory(source, newDirectory);
    } 
}

//rm function
void rm(fileSys **files, char *path){
    if (strncmp(path, "-r", 2) == 0) {
        // Move the pointer past "-r" and any spaces
        // Skip "-r"
        path += 3; 
    }
    char **storePaths = makeCharArray2D(256, 1024);
    int pathTotal = 0;
    // Tokenize the string by spaces to get each file path
    // Get the first token
    char *token = strtok(path, " "); 

    while (token != NULL) {
        strcpy(storePaths[pathTotal], token);
        // Increment the path counter
        pathTotal++; 
        // Get the next file path
        // Get the next token
        token = strtok(NULL, " "); 
    }
    pathTotal--;
    while(pathTotal >= 0){
        char *pathPart = storePaths[pathTotal];
        // Handle nested directory creation (path contains "/")
        file *originalActive = (*files)->active;
        // Search for the parent directory using the modified path
        file *target = searchFile(files, (*files)->active, pathPart);
        if (target == NULL) {
            pathTotal--;
            continue;  
            // Skip this entry if the file is not found
        }
        removeFile(target, pathPart);
        // Reset the active directory back to the original
        (*files)->active = originalActive;
        pathTotal--;
    }
    destroyArray2D(storePaths, 256);
}

//ln function
void ln(fileSys **fs, char *input){
    char **storePaths = makeCharArray2D(2, 1024);
    int pathTotal = 0;
    // Tokenize the string by spaces to get each file path
    // Get the first token
    char *token = strtok(input, " "); 
    while (token != NULL) {
        strcpy(storePaths[pathTotal], token);
        // Increment the path counter
        pathTotal++;
        // Get the next file path
        // Get the next token
        token = strtok(NULL, " "); 
    }
    //get the existing regular file
    file *sFile =searchFile(fs, (*fs)->active, storePaths[0]);
    if(sFile == NULL || sFile->isDirectory == 1){
        return;
    }
    //path to the file that we want to link
    char destPath[1024];
    strcpy(destPath, storePaths[1]);
    char *dName = strrchr(destPath, '/');
    char path[1024];
    if(dName!=NULL){
        *dName = '\0';
        dName++;
        strncpy(path, destPath, sizeof(path)-1);
        path[sizeof(path)-1] = '\0';
    }
    else{
        dName = destPath;
        // No parent path
        strcpy(path, ""); 
    }
    //make new file and copy the content
    file *parentDirectory = searchFile(fs, (*fs)->active, path);
    file *newHL = createFile(dName, 0, parentDirectory, NULL, sFile->inode);
    newHL->numChild = 0;
    strcpy(newHL->content.data, sFile->content.data);
    //replace the copy in the parent directory
    for(int i=0; i<256; i++){
        if(parentDirectory->content.children[i] == NULL){
            parentDirectory->content.children[i] = newHL;
            parentDirectory->numChild++;
            break;
        }
    }
    destroyArray2D(storePaths, 2);
}