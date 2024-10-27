#include "build.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "config.h"

char** src_files;
uint8_t src_files_counter = 0;

char** obj_files;
uint8_t obj_files_counter = 0;

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

bool compile(char* path) {

    // get enviroment variable "LIBADI_PATH"
    char* libadi_path = getenv("LIBADI_PATH");
    if (libadi_path == NULL) {
        printf("ERROR: LIBADI_PATH is not set\n");
        return false;
    }

    //WARNING: this is hacky and WILL ONLY WORK if PATH LOOKS LIKE "dir/src_dir..."
    char bin_path[1024];
    strcpy(bin_path, path);
    strcpy(strchr(bin_path, '/') + 1, "bin");
    strcat(bin_path, strchr(strchr(path, '/') + 1, '/'));
    strcat(bin_path, ".o");

    obj_files = (char**)realloc(obj_files, sizeof(char*) * (obj_files_counter + 1));
    obj_files[obj_files_counter] = strdup(bin_path);
    obj_files_counter++;

    char command[1024];
    strcpy(command, "clang");
    strcat(command, " -I");
    strcat(command, libadi_path);
    strcat(command, " -target ");
    strcat(command, cross_arch);
    strcat(command, " -c ");
    strcat(command, path);
    strcat(command, " -o ");
    strcat(command, bin_path);

    return (system(command) == 0);
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


    char bindir[1024];
    strcpy(bindir, directory);
    strcat(bindir, "/bin");

    if(access(bindir, F_OK) != 0) {
        if(mkdir(bindir, 0777) != 0) {
            printf("ERROR: Failed to create bin directory\n");
            goto error;
        }
    }

    obj_files = (char**)malloc(sizeof(char*));

    for(uint8_t i = 0; src_files[i]; i++) {
        printf("Compiling: %s\n", src_files[i]);
        if(!compile(src_files[i])) {
            printf("ERROR: Failed to compile: %s\n", src_files[i]);
            goto error;
        }
    }

    obj_files[obj_files_counter] = NULL;

    printf("Linking driver.elf\n");

    char link_cmd[2048];
    strcpy(link_cmd, "ld ");
    for(uint8_t i = 0; obj_files[i]; i++) {
        strcat(link_cmd, obj_files[i]);
        strcat(link_cmd, " ");
    }
    strcat(link_cmd, " -o ");
    strcat(link_cmd, bindir);
    strcat(link_cmd, "/driver.elf");

    if(system(link_cmd) != 0) {
        printf( "ERROR: Failed to link\n");
        goto error;
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