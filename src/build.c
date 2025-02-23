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
#include "convert.h"

char** src_files;
uint8_t src_files_counter = 0;

char** obj_files;
uint8_t obj_files_counter = 0;

void free_src_files() {
    for (uint8_t i = 0; i < src_files_counter; i++) {
        free(src_files[i]);
    }
    free(src_files);
}

// Free memory allocated by strdup for obj_files
void free_obj_files() {
    for (uint8_t i = 0; i < obj_files_counter; i++) {
        free(obj_files[i]);
    }
    free(obj_files);
}

bool explore_dir(char* dirname) {
    DIR* dir = opendir( dirname);
    if (dir == NULL) {
        printf("\e[1;31merror\e[0m: Failed to open source directory\n");
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

bool compile(const char* path) {
    const char* libadi_path = getenv("LIBADI_PATH");
    if (!libadi_path) {
        if(access("/usr/local/include/libadi", F_OK) == 0) {
            libadi_path = "/usr/local/include/libadi";
        } else {
            fprintf(stderr, "\e[1;31merror\e[0m: LIBADI_PATH environment variable is not set\n");
            return false;
        }
    }

    char* first_slash = strchr(path, '/');
    if (!first_slash) {
        fprintf(stderr, "\e[1;31merror\e[0m: Invalid path format: %s\n", path);
        return false;
    }

    char bin_path[1024];
    const char *bin_dir = "bin/";
    const char *filename = strrchr(path, '/');
    
    if (filename) {
        filename++;
    } else {
        filename = path;
    }
    
    snprintf(bin_path, sizeof(bin_path), "%s%s.o", bin_dir, filename);
    

    char** temp = realloc(obj_files, sizeof(char*) * (obj_files_counter + 1));
    if (!temp) {
        fprintf(stderr, "\e[1;31merror\e[0m: Memory allocation failed\n");
        return false;
    }
    obj_files = temp;
    obj_files[obj_files_counter++] = strdup(bin_path);

    const char* compiler = getenv("ADIKIT_CC");
    if (!compiler) {
        printf("\e[1;33mwarning\e[0m: ADIKIT_CC environment variable is not set, using /bin/cc\n");
        compiler = "cc";
    }

    char *comp_specifc_arg_string = malloc(1024);

    if (strstr(compiler, "clang") != NULL) {
        snprintf(comp_specifc_arg_string, 1024, "-target %s", cross_arch);
    } else {
        strcpy(comp_specifc_arg_string, "    ");
    }

    char* command = malloc(strlen(compiler) + strlen(libadi_path) + strlen(path) + strlen(bin_path) + 64);

    if (!command) {
        fprintf(stderr, "\e[1;31merror\e[0m: Memory allocation failed\n");
        return false;
    }

    sprintf(command, "%s -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -I%s -g -c %s -o %s %s", compiler, libadi_path, path, bin_path, comp_specifc_arg_string);

    int result = system(command);
    free(command);
    free(comp_specifc_arg_string);

    return result == 0;
}

bool build(char* directory) {
    static bool result = true;
    if (directory == NULL) {
        goto error;
    }

    if (access(directory, F_OK) != 0) {
        printf("\e[1;31merror\e[0m: Failed to find directory\n");
        return false;
    }
    char drvdesc[1024];
    strcpy(drvdesc, directory);
    strcat(drvdesc, "/.drvdesc");
    FILE* file = fopen(drvdesc, "r");
    if (file == NULL) {
        printf("\e[1;31merror\e[0m: Failed to open .drvdesc file\n");
        goto error;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    char* buffer = malloc(size + 1);
    if (buffer == NULL) {
        printf("\e[1;31merror\e[0m: Failed to allocate memory\n");
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
        printf("\e[1;31merror\e[0m: Failed to find source directory\n");
        goto error;
    }


    char newsrcdir[1024] = {0};
    strcpy(newsrcdir, directory);
    strcat(newsrcdir, "/");
    strcat(newsrcdir, src_dir);

    src_files = (char**)malloc(sizeof(char*) * src_files_counter);
    explore_dir(newsrcdir);


    char bindir[1024];
    strcpy(bindir, directory);
    strcat(bindir, "/bin");

    if(access(bindir, F_OK) != 0) {
        if(mkdir(bindir, 0777) != 0) {
            printf("\e[1;31merror\e[0m: Failed to create bin directory\n");
            goto error;
        }
    }

    obj_files = (char**)malloc(sizeof(char*) * obj_files_counter);

    for(uint8_t i = 0; i < src_files_counter; i++) {
        printf("Compiling: %s\n", src_files[i]);
        if(!compile(src_files[i])) {
            printf("\e[1;31merror\e[0m: Failed to compile: %s\n", src_files[i]);
            goto error;
        }
    }

    printf("Linking driver.elf\n");

    char link_cmd[2048];
    strcpy(link_cmd, "ld ");
    for(uint8_t i = 0; i < obj_files_counter; i++) {
        strcat(link_cmd, obj_files[i]);
        strcat(link_cmd, " ");
    }
    strcat(link_cmd, " -T ");
    strcat(link_cmd, directory);
    strcat(link_cmd, "/linker.ld");

    strcat(link_cmd, " -o ");
    strcat(link_cmd, bindir);
    strcat(link_cmd, "/driver.elf");

    if(system(link_cmd) != 0) {
        printf( "\e[1;31merror\e[0m: Failed to link\n");
        goto error;
    }

    char driver_elf[1024];
    strcpy(driver_elf, bindir);
    strcat(driver_elf, "/driver.elf");

    if(!convert(driver_elf)) {
        printf("\e[1;31merror\e[0m: Failed to convert driver.elf\n");
        goto error;
    }

    driver_elf[strlen(driver_elf) - 3] = '\0';
    strcat(driver_elf, "adi");


    char output_adi[1024];
    strcpy(output_adi, directory);
    strcat(output_adi, "/output.adi");

    if(access(output_adi, F_OK) == 0) {
        if(remove(output_adi) != 0) {
            printf("\e[1;31merror\e[0m: Failed to remove output.adi\n");
            goto error;
        }
    }

    if(rename(driver_elf, output_adi) != 0) {
        printf("\e[1;31merror\e[0m: Failed to rename driver.adi to output.adi\n");
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
    free(metalangs_used);

    free_obj_files();
    free_src_files();

    return result;

    error:

    result = false;


    goto finish;
}