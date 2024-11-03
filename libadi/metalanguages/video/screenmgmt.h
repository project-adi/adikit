#ifndef __SCREENMGMNT_H__
#define __SCREENMGMNT_H__

#include <stdint.h>

#include <core.h>

#define COLOR_CONFIG_RGB565 0
#define COLOR_CONFIG_RGB888 1
#define COLOR_CONFIG_ARGB8888 2
#define COLOR_CONFIG_BGRA8888 3

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t pitch;

    uint8_t color_config;
} modeinfo_t;

typedef struct {
    modeinfo_t* modes;
    uint32_t num_modes;

    uint32_t current_mode;
    uint32_t fb_id;
} screen_t;

typedef struct {
    screen_t* screens;
    uint32_t num_screens;
} screen_list_t;

typedef screen_list_t (*event_get_screens)(adi_device_t* dev);
typedef bool (*event_set_mode)(uint32_t fbid,uint32_t mode);


typedef struct {
    metalanguage_t (*new)( event_get_screens get_screens,event_set_mode set_mode);
} metalang_x86_64_t;

#endif // __SCREENMGMNT_H__