#include "build.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#include "config.h"

char** src_files;
uint8_t src_files_counter = 0;

bool explore_dir(char* dirname) {
    DIR* dir = opendir( dirname);
    if (dir == NULL) {
        printf("ERROR: Failed to open source directory\n");
        return false;
    }

    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
        char* filename = ent->d_name;

        if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
            continue;
        }

        if (ent->d_type == DT_DIR) {
            char path[1024];
            strcpy(path, dirname);
            strcat(path, "/");
            strcat(path, filename);
            explore_dir(path);
            continue;
        }

        if(strlen(filename) >= 3) {
            if (filename[strlen(filename) - 2] == '.' && filename[strlen(filename) - 1] == 'c') {
                src_files = (char**)realloc(src_files, sizeof(char*) * (src_files_counter + 1));
                
                char path[1024];
                strcpy(path, dirname);
                strcat(path, "/");
                strcat(path, filename);

                src_files[src_files_counter] = strdup(path);
                src_files_counter++;
            }
        }
    }
    closedir(dir);

    return true;
}

bool build(char* directory) {
    static bool result = true;
    if (directory == NULL) {
        goto error;
    }

    if (access(directory, F_OK) != 0) {
        printf("ERROR: Failed to find directory\n");
        return false;
    }
    char drvdesc[1024];
    strcpy(drvdesc, directory);
    strcat(drvdesc, "/.drvdesc");
    FILE* file = fopen(drvdesc, "r");
    if (file == NULL) {
        printf("ERROR: Failed to open .drvdesc file\n");
        goto error;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    char* buffer = malloc(size + 1);
    if (buffer == NULL) {
        printf("ERROR: Failed to allocate memory\n");
        goto error;
    }
    fread(buffer, 1, size, file);
    buffer[size] = '\0';

    fclose(file);

    if(!parse_config(buffer))
    {
        //there is already an error printed
        goto error;
    }

    if (access(src_dir, F_OK) != 0) {
        printf("ERROR: Failed to find source directory\n");
        goto error;
    }


    char newsrcdir[1024] = {0};
    strcpy(newsrcdir, directory);
    strcat(newsrcdir, "/");
    strcat(newsrcdir, src_dir);

    src_files = (char**)malloc(sizeof(char*));

    explore_dir(newsrcdir);

    src_files[src_files_counter] = NULL;

    for(uint8_t i = 0; src_files[i]; i++) {
        char* filename = src_files[i];
        printf("Got %s\n", filename);
    }


    finish:

    free(buffer);
    free(name);
    free(author);
    free(cross_arch);
    free(src_dir);
    for(uint8_t i = 0; metalangs_implemented[i]; i++) {
        free(metalangs_implemented[i]);
    }
    free(metalangs_implemented);
    for(uint8_t i = 0; metalangs_used[i]; i++) {
    }
    free(metalangs_used);

    return result;

    error:

    result = false;

    goto finish;
}