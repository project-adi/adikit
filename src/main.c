#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "create.h"
#include "build.h"

#define US_CODE_NO_ACTIO 0
#define US_CODE_BAD_ACTIO 1

#define ACTIO_INVALID  0
#define ACTION_CREATE  1
#define ACTION_COMPILE 2

char actio = 0;

void usage(int errcode) {
    printf("Usage: main <action>\n");
    switch(errcode) {
        case US_CODE_NO_ACTIO:
            printf("ERROR: No action specified\n");
            break;
        case US_CODE_BAD_ACTIO:
            printf("ERROR: Bad action specified\n");
            break;
    }
    exit(0);
}

bool parse_args(int argc, char **argv) {
    char* astr = argv[1];
    if(astr == NULL) {
        usage(US_CODE_NO_ACTIO);
    }else if(strcmp(astr, "create") == 0) {
        actio = ACTION_CREATE;
    }else if (strcmp(astr, "build") == 0) {
        actio = ACTION_COMPILE;
    }else {
        usage(US_CODE_BAD_ACTIO);
    } 

    return true;
}

int main(int argc, char **argv) {
    if(!parse_args(argc, argv)) {
        printf("Failed to parse arguments\n");
        return 1;
    }

    switch (actio) {
        case ACTION_CREATE:
            create(argv[2]);
            break;
        case ACTION_COMPILE:
            build(argv[2]);
            break;
    }
    return 0;
}