#ifndef __TIMEKEEPER_H__
#define __TIMEKEEPER_H__
#include <cstdint>
#include <stdint.h>

#include <core.h>
#include <sys/types.h>

typedef struct {
    uint32_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t usec;
    uint8_t nsec;
} time_t;

typedef time_t (*event_get_time)(adi_device_t* dev);
typedef bool   (*event_set_time)(adi_device_t* dev,time_t* time);
typedef uint64_t (*event_get_time_unix)(adi_device_t* dev);
typedef uint64_t (*event_set_time_unix)(adi_device_t* dev,uint64_t time);

typedef struct {
    metalanguage_t (*new)(event_get_time get_time,event_set_time set_time,event_get_time_unix get_time_unix,event_set_time_unix set_time_unix);
} metalang_timekeeper_t;
#endif // __TIMEKEEPER_H__