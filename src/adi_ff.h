#ifndef __ADI_FF_H__
#define __ADI_FF_H__

#include <stdint.h>
#include <endian.h>

#define ADI_FF_MAGIC 0x4C4F4F4B20494441

#define ADI_FF_SEGMENT_FLAG_EXEC      1 >> 0
#define ADI_FF_SEGMENT_FLAG_WRITE     1 >> 1
#define ADI_FF_SEGMENT_FLAG_READ      1 >> 2
#define ADI_FF_SEGMENT_FLAG_INFILE    1 >> 3
#define ADI_FF_SEGMENT_FLAG_PAGING    1 >> 4
#define ADI_FF_SEGMENT_FLAG_META      1 >> 5
#define ADI_FF_SEGMENT_FLAG_USERSPACE 1 >> 6
#define ADI_FF_SEGMENT_FLAG_RESERVED  1 >> 7

typedef struct {
    uint64_t magic_num;
    uint16_t spec_version;
    uint64_t entry_point;
    uint16_t name_offset;
    uint16_t author_offset;
    uint8_t ver_major;
    uint8_t ver_minor;
    uint8_t ver_patch;
    uint32_t metalang_table_offset;
    uint32_t metalang_table_size;
    uint32_t string_table_offset;
    uint32_t string_table_size;
    uint32_t segment_table_offset;
    uint32_t segment_table_size; 
    uint32_t content_region_offset;
    uint64_t cfr_addr;
} __attribute__((packed)) adi_ff_header_t;

typedef struct {
    uint16_t name_offset;
    uint32_t segment_offset;
    uint32_t segment_size;
    uint64_t virt_addr;
    uint8_t flags;
} __attribute__((packed)) adi_ff_segment_t;

typedef struct {
    uint32_t id;
    uint64_t pointer_addr;
} __attribute__((packed)) adi_ff_metalang_t;

#endif // __ADI_FF_H__