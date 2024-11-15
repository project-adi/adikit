#include "config.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

char* name;
uint8_t name_len = 0;
char* author;
uint8_t author_len = 0;
char* cross_arch;
uint8_t cross_arch_len = 0;
driver_perms_t permissions = {0};
bool require_kernel = false;
uint8_t ver_major = 1;
uint8_t ver_minor = 0;
uint8_t ver_patch = 0;
char* src_dir;
uint8_t src_dir_len = 0;
char** metalangs_implemented;
char** metalangs_used;

//TODO: make the desired ADI version actually specifiable
uint16_t adi_version = 1;

bool parse_config(char* drvdesc) {
    char* line = drvdesc;
    uint8_t impl_counter = 0;
    metalangs_used = (char**)malloc(sizeof(char*));
    uint8_t used_counter = 0;
    metalangs_implemented = (char**)malloc(sizeof(char*));

    while (1) {
        char* tonull = strchr(line, '\n');
        if (tonull == NULL) {
            break;
        }

        *tonull = '\0';

        if(line[0] == '#') {
            goto next_line;
        }

        if (strncmp("name: ", line, 6) == 0) {
            name = strdup(line + 6);
        } else if (strncmp("author: ", line, 8) == 0) {
            author = strdup(line + 8);
        } else if (strncmp("cross-arch: ", line, 12) == 0) {
            cross_arch = strdup(line + 12);
        } else if (strncmp("permissions: ", line, 13) == 0) {
            for(int i = 12; i < strlen(line); i++) {
                if(line[i] == ' ') {
                    continue;
                } else if(line[i] == 'P') {
                    permissions |= PERMS_PAGING;
                } else if(line[i] == 'M') {
                    permissions |= PERMS_METADRIVER;
                } else if(line[i] == 'U') {
                    permissions |= PERMS_USERSPACE;
                }

                if(line[i] == 'n' && line[i + 1] == 'o' && line[i + 2] == 'n' && line[i + 3] == 'e') {
                    break;
                }
            }
        } else if (strncmp("require_kernel: ", line, 16) == 0) {
            if (strcmp(line + 16, "true") == 0) {
                require_kernel = true;
            } else if (strcmp(line + 16, "false") == 0) {
                require_kernel = false;
            }
        } else if (strncmp("src_dir: ", line, 9) == 0) {
            src_dir = strdup(line + 9);
        } else if (strncmp("version: ", line, 9) == 0) {
            char* verstr = strchr(line + 9, 'v') + 1;
            if (verstr == NULL) {
                printf("ERROR: Failed to parse version string\n");
                return false;
            }

            if (sscanf(verstr, "%hhu.%hhu.%hhu", &ver_major, &ver_minor, &ver_patch) != 3) {
                printf("ERROR: Failed to parse version string\n");
                return false;
            }
        } else if (strncmp("implements: ", line, 11) == 0) {
            impl_counter = 1;
        } else if (strncmp("uses: ", line, 6) == 0) {
            used_counter = 1;
        } else if (strncmp("    ", line, 4) == 0 || strncmp("  ", line, 2) == 0 || strncmp("\t", line, 1) == 0) {
            if (impl_counter > 0) {
                metalangs_implemented[impl_counter - 1] = strdup(line + 2);
                impl_counter++;
                metalangs_implemented = (char**)realloc(metalangs_implemented, sizeof(char*) * impl_counter);
            } else if (used_counter > 0) {
                metalangs_used[used_counter - 1] = strdup(line + 2);
                used_counter++;
                metalangs_used = (char**)realloc(metalangs_used, sizeof(char*) * used_counter);
            }
        }

        next_line:
        *(strchr(line, 0)) = '\n';
        line = strchr(line, '\n');
        if(line == NULL) {
            break;
        }
        line++;
    }

    if (name == NULL || author == NULL || src_dir == NULL) {
        printf("ERROR: Mandatory fields(name, author, src_dir) not present\n");
        return false;
    }

    if (cross_arch == NULL) {
        cross_arch = strdup("x86_64-pc-none-elf");
    }

    if(impl_counter == 0) {
        impl_counter = 1;
    }

    if(used_counter == 0) {
        used_counter = 1;
    }

    metalangs_implemented[impl_counter - 1] = NULL;
    metalangs_used[used_counter - 1] = NULL;

    return true;
}