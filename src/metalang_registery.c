#include "metalang_registery.h"

#include <string.h>

//TODO: unbodge
uint32_t get_metalang_id(char* name) {
    if(strcmp(name, "arch_x86_64") == 0) {
        return 0x00000001;
    } else if(strcmp(name, "bus_pci") == 0) {
        return 0x00010000;
    } else if(strcmp(name, "hid_kb") == 0) {
        return 0x40010000;
    } else if(strcmp(name, "hid_pointer") == 0) {
        return 0x40010001;
    } else if(strcmp(name, "video_screenmgmnt") == 0) {
        return 0x40030000;
    } else if(strcmp(name, "video_fb") == 0) {
        return 0x40030001;
    } else if(strcmp(name, "misc_storage") == 0) {
        return 0x400F0000;
    } else if(strcmp(name, "misc_timekeeper") == 0) {
        return 0x400F0001;
    } else {
        return 0xFFFFFFFF;
    }
}