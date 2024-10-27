#ifndef __ADI_CORE_H__
#define __ADI_CORE_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    uint32_t id;
    char* name;

    void* lang_impl;
    void* params;
} metalanguage_t;

typedef struct {
    uint32_t id;
    char* name;
    metalanguage_t** metalangs_implemented;
    metalanguage_t** metalangs_used;
} adi_device_t;

typedef struct {
    //Logging API
    void (*log_info)(char* format,...);
    void (*log_warning)(char* format,...);
    void (*log_error)(char* format,...);
    void (*panic)(char* format,...);

    //Device API
    uint32_t (*register_device)(metalanguage_t* metalanguages_implemented,uint32_t metalanguage_count);
    void (*unregister_device)(uint32_t device_id);

    // Memory API
    void* (*alloc)(size_t size);
    bool  (*free)(void* ptr);
    void* (*realloc)(void* ptr, size_t size);

    bool (*memcpy)(void* dst, void* src, size_t size);
    bool (*memset)(void* dst, uint8_t value, size_t size); 

} adi_core_t;
#endif // __ADI_CORE_H__