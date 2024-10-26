#include "config.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>


char* name;
uint8_t name_len;
char* author;
uint8_t author_len;
char* cross_arch;
uint8_t cross_arch_len;
driver_perms_t permissions;
bool require_kernel;
char* src_dir;
uint8_t src_dir_len;
char** metalangs_implemented;
char** metalangs_used;

bool parse_config(char* drvdesc) {
    char* line = drvdesc;

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
        } else if (strncmp("implements: ", line, 11) == 0) {
            //metalangs_implemented = strdup(line + 11);
        } else if (strncmp("uses: ", line, 6) == 0) {
            //metalangs_used = strdup(line + 6);
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
        cross_arch = "x86_64-pc-none-elf";
    }

    printf("name: %s\n", name);
    printf("author: %s\n", author);
    printf("cross-arch: %s\n", cross_arch);
    printf("permissions: %d\n", permissions);
    printf("require_kernel: %s\n", require_kernel ? "true" : "false");
    printf("src_dir: %s\n", src_dir);

    return true;
}