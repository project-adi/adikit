#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t driver_perms_t;

#define PERMS_NONE 0
#define PERMS_PAGING 1
#define PERMS_METADRIVER 2
#define PERMS_USERSPACE 4

extern char* name;
extern char* author;
extern char* cross_arch;
extern driver_perms_t permissions;
extern bool require_kernel;
extern uint8_t ver_major;
extern uint8_t ver_minor;
extern uint8_t ver_patch;
extern char* src_dir;
extern char** metalangs_implemented;
extern char** metalangs_used;

extern uint16_t adi_version;

bool parse_config(char* drvdesc);

#endif // __CONFIG_H__