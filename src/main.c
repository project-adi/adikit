#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "create.h"
#include "build.h"
#include "util.h"

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
            printf("ADIKit Help command\n");
            printf("\tBasic Usage: %s [action] [args]\n", argv[0]);
            printf("\n");
            printf("Actions:\n");
            printf("\tcreate\t\tCreate a new driver projects\n");
            printf("\tbuild\t\tBuild a driver project\n");
            printf("\n");

            printf("Options:\n");
            printf("\t-h, --help\t\tShow this help message\n");
            printf("\t-v, --version\t\tShow version information\n");

            printf("Building:\n");
            printf("\tTo build a driver project, run %s build [path to driver project]\n", argv[0]);
            printf("\n");

            exit(0);
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            char* buffer = malloc(256);
            generate_version_string(buffer);

            printf("%s\n", buffer);
            free(buffer);
            printf("Copyright (C) 2025 Adaptive Driver Interface Project.\nThis work has been released under CC0 1.0 Universal (Public Domain Dedication). You may copy, modify, and distribute it without any restrictions.\n\n");
            printf("This is free software; see the source for copying conditions.  There is NO\nwarranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");

            exit(0);
        }
    }

    if(astr == NULL) {
        usage(US_CODE_NO_ACTIO, argv[0]);
    } else if(strcmp(astr, "create") == 0) {
        actio = ACTION_CREATE;
    } else if (strcmp(astr, "build") == 0) {        
        if (argc < 3) {
            printf("\e[1;31merror\e[0m: Not enough args for action build\n       For help run %s --help\n", argv[0]);
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