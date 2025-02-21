#include "util.h"

#include <stdio.h>
#include <sys/utsname.h>

void generate_version_string(char *buffer) {
    struct utsname sys_info;

    if (uname(&sys_info) != 0) {
        sprintf(buffer, "adikit (%d.%d.%d generic-unix-os) %d.%d.%d", ADIKIT_VERSION_MAJOR, ADIKIT_VERSION_MINOR, ADIKIT_VERSION_PATCH, ADIKIT_VERSION_MAJOR, ADIKIT_VERSION_MINOR, ADIKIT_VERSION_PATCH);
        return;
    }

    sprintf(buffer, "adikit (%d.%d.%d %s %s-%s) %d.%d.%d", ADIKIT_VERSION_MAJOR, ADIKIT_VERSION_MINOR, ADIKIT_VERSION_PATCH, sys_info.sysname, sys_info.release, sys_info.machine, ADIKIT_VERSION_MAJOR, ADIKIT_VERSION_MINOR, ADIKIT_VERSION_PATCH);
}