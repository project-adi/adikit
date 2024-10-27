#ifndef __ADI_FF_H__
#define __ADI_FF_H__

#include <stdint.h>
#include <endian.h>

#define ADI_FF_MAGIC le64toh(0x414449204B4F4F4C)

#define ADI_FF_SECTION_FLAG_EXEC      1 >> 0
#define ADI_FF_SECTION_FLAG_WRITE     1 >> 1
#define ADI_FF_SECTION_FLAG_READ      1 >> 2
#define ADI_FF_SECTION_FLAG_INFILE    1 >> 3
#define ADI_FF_SECTION_FLAG_PAGING    1 >> 4
#define ADI_FF_SECTION_FLAG_META      1 >> 5
#define ADI_FF_SECTION_FLAG_USERSPACE 1 >> 6
#define ADI_FF_SECTION_FLAG_RESERVED  1 >> 7

typedef struct {
    uint64_t magic_num;
    uint16_t ISA;
    uintptr_t entry_point;
    uint16_t name_offset;
    uint16_t author_offset;
    struct {
        uint8_t major;
        uint8_t minor;
        uint8_t patch;
    } version;
   uint32_t metalang_table_offset;
   uint32_t string_table_offset;
   uint32_t segment_table_offset; 
   uint32_t content_region_offset;
   uintptr_t cfr_addr;
} __attribute__((packed)) adi_ff_header_t;

typedef struct {
    uint16_t name_offset;
    uint32_t segment_offset;
    uint32_t segment_size;
    uint8_t flags;
} __attribute__((packed)) adi_ff_segment_t;

typedef struct {
    uint32_t id;
    uintptr_t pointer_addr;
} __attribute__((packed)) adi_ff_metalang_t;

#endif // __ADI_FF_H__