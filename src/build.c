#include "build.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "config.h"

bool build(char* directory) {
    if (directory == NULL) {
        return false;
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
        return false;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    char* buffer = malloc(size + 1);
    if (buffer == NULL) {
        printf("ERROR: Failed to allocate memory\n");
        return false;
    }
    fread(buffer, 1, size, file);
    buffer[size] = '\0';

    fclose(file);

    parse_config(buffer);


    return true;
}