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
    if(active->isDirectory == false  || active->numChild == 0) {
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

void touch(file *active, char *path) {
    char *token = strtok(filename, " ");  // Get the first filename
    while (token != NULL) {
        int ok = 0;
        for(int i = 0; i <= active->numChild; i++){
            if(active->content.children[i] == token){
                ok = 1;
                break;
            }
        }
        // Create an empty file in the current directory
        if(ok == 0){
        insertFile(active, token, 0, NULL);
        }
        token = strtok(NULL, " ");  // Get the next filename
    }
}

void echo(fileSys **files, file *active char *input){
    //get string that needs to be put in file
    char *token = strtok(input, "\"");
    //if no string return
    if(token == NULL){
        return;
    }
    //keep string in variable
    char *content = token;
    token = strtok(NULL, " ");
    //check which type of echo we have
    if(strlen(token) == 1){
        token = strtok(NULL, " ");
        if(name == NULL){
            return;
        }
        int ok=-1;
        for(int i=0; i <= (*files)->active->numChild; i++){
            if(strcmp((*files)->active->content.children[i]->name, token)==0){
                ok=i;
                break;
            }
        }
        if(ok!=-1){
            token = 
        }
        else{

        }
    }
    else{
        //echo >>
    }


}

void mkdir(fileSys **files, char *path){
    char *token = strtok(path, " "); 

    if (token != NULL && strcmp(token, "-p") == 0) {
        path = strtok(NULL, " ");  // Skip the -p argument and get the actual path
    }
    // Handle nested directory creation (path contains "/")
    file *originalActive = (*files)->active;

    // Handle the case when the path doesn't contain "/"
    if (strchr(path, '/') == NULL) {
        // This means it's just a single directory name
        file *existingDir = searchFile((*files)->active, path);
        if (existingDir == NULL) {
            originalActive->numChild++;
            insertFile((*files)->active, path, 1, NULL);  // '1' indicates it's a directory
            printf("Created directory: %s in %s\n", path, (*files)->active->name);
        } 
    } else {

        // Handle nested directory creation (path contains "/")
        token = strtok(path, "/");  // Tokenize the path by "/"
        
        while (token != NULL) {
            // Check if the directory exists in the currentDir
            file *existingDir = searchFile((*files)->active, token);

            if (existingDir == NULL) {
                // If the directory doesn't exist, create it
                insertFile((*files)->active, token, 1, NULL);  // '1' indicates it's a directory
                printf("Created directory: %s in %s\n", token, (*files)->active->name);
            } else {
                // If the directory exists, move into it and continue
                (*files)->active = existingDir;
            }

            // Get the next directory in the path
            token = strtok(NULL, "/");
        }
    }
    // Reset the active directory back to the original
    (*files)->active = originalActive;
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