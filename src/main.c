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

int actio = 0;

void usage(int errcode, char* exec_name) {
    switch(errcode) {
        case US_CODE_NO_ACTIO:
            printf("\e[1;31merror\e[0m: No action specified\n       For help run %s --help\n", exec_name);
            break;
        case US_CODE_BAD_ACTIO:
            printf("\e[1;31merror\e[0m: Bad action specified\n       For help run %s --help\n", exec_name);
            break;
    }
    exit(1);
}

bool parse_args(int argc, char **argv) {
    char* astr = argv[1];

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("funny help menu which isnt helpful!\n");
            exit(0);
        }
    }

    if(astr == NULL) {
        usage(US_CODE_NO_ACTIO, argv[0]);
    } else if(strcmp(astr, "create") == 0) {
        actio = ACTION_CREATE;
    } else if (strcmp(astr, "build") == 0) {        
        if (argc < 3) {
            printf("\e[1;31merror\e[0m: Not enough args for build\n       For help run %s --help\n", argv[0]);
            exit(1);
        }
        actio = ACTION_COMPILE;
    } else {
        usage(US_CODE_BAD_ACTIO, argv[0]);
    }

    return true;
}

int main(int argc, char **argv) {
    if(!parse_args(argc, argv)) {
        printf("\e[1;31merror\e[0m: Couldn't parse arguments\n");
        return 1;
    }

    switch (actio) {
        case ACTION_CREATE:
            create();
            break;
        case ACTION_COMPILE:
            build(argv[2]);
            break;
    }
    return 0;
}