#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <stdint.h>

#include <core.h>

typedef struct {
    uint64_t size;
    uint64_t sector_size;
    bool read_only;
    //TODO: CD burning support
} sdev_ident_t;

typedef sdev_ident_t (*event_identify)(adi_device_t* dev);
typedef uint32_t (*event_transact)(adi_device_t* dev,bool write,uint32_t offset,uint32_t count,void* buffer); 

typedef struct {
    metalanguage_t (*new)(event_identify identify,event_transact transact);
    bool (*signal_transaction_done)(adi_device_t* dev, uint32_t id);

} metalang_storage_t;

#endif // __STORAGE_H__