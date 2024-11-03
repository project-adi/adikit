#ifndef __KB_H__
#define __KB_H__

#include <stdint.h>

#include <core.h>

typedef void (*poll_handler)(adi_device_t* dev,char* buffer,uint32_t len);

typedef struct {
    metalanguage_t (*new)(poll_handler event_poll);
    bool send_event_key(adi_device_t* dev,char scancode);
} metalang_kb_t;

#endif // __KB_H__