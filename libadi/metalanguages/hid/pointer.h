#ifndef __POINTER_H__
#define __POINTER_H__
#include <cstdint>
#include <stdint.h>

#include <core.h>
#include <sys/types.h>

typedef struct {
    metalanguage_t (*new)();
    bool (*send_delta)(adi_device_t* dev,uint32_t dx,uint32_t dy);
    bool (*send_button)(adi_device_t* dev,uint32_t button,bool down);
} metalang_pointer_t;
#endif // __POINTER_H__