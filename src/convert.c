#include "convert.h"

#include <elf.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <endian.h>
#include <libelf.h>
#include <gelf.h>

#include "adi_ff.h"
#include "config.h"
#include "metalang_registery.h"

adi_ff_header_t header = {0};
char* string_table = NULL;
uint32_t string_table_size = 0;
char* metalang_table = NULL;
uint32_t metalang_table_size = 0;
char* segment_table = NULL;
uint32_t segment_table_size = 0;

char* segment_contents = NULL;
uint32_t segment_contents_size = 0;

bool prepare_string_table(){
    //TODO: use the string table for more
    string_table_size = strlen(name) + strlen(author) + 2;
    string_table = (char*) malloc(string_table_size);
    memset(string_table, 0, string_table_size);
    strcpy(string_table, name);
    string_table[strlen(name)] = '\0';
    strcpy(string_table + strlen(name) + 1, author);
    string_table[string_table_size - 1] = '\0';

    header.name_offset = 0;
    header.author_offset = htole16(strlen(name) + 1);

    return true;
}
 
bool append_metalang(Elf* elf, char* metalang){
    adi_ff_metalang_t mlang = (adi_ff_metalang_t){
        .id = htole32(get_metalang_id(metalang)),
    };
    metalang_table_size += sizeof(mlang);
    metalang_table = (char*)realloc(metalang_table, metalang_table_size);
    if(metalang_table == NULL){
        printf("\e[1;31merror\e[0m: Failed to allocate memory\n");
        return false;
    }

    memcpy(metalang_table + metalang_table_size - sizeof(mlang), &mlang, sizeof(mlang));

    return true;
}

bool prepare_metalang_table(Elf* elf){
    metalang_table_size = 0;

    for(int i = 0; metalangs_used[i]; i++){
        append_metalang(elf,metalangs_used[i]);
    }

    for(int i = 0; metalangs_implemented[i]; i++){
        append_metalang(elf,metalangs_implemented[i]);
    }

    return true;
}

bool append_segment(Elf* elf, Elf_Scn* scn,size_t shstrndx){
    GElf_Shdr shdr;
    if (gelf_getshdr(scn, &shdr) != &shdr) {
        fprintf(stderr, "gelf_getshdr failed: %s\n", elf_errmsg(-1));
        return false;
    }

    //get the segment name
    const char *name = elf_strptr(elf, shstrndx, shdr.sh_name);

    if(strcmp(name, ".comment") == 0 || strcmp(name, ".strtab") == 0 || strcmp(name, ".shstrtab") == 0){
        return true;
    }

    string_table_size += strlen(name) + 1;
    string_table = (char*)realloc(string_table, string_table_size);
    if(string_table == NULL){
        printf("\e[1;31merror\e[0m: Failed to allocate memory\n");
        return false;
    }
    strcpy(string_table + string_table_size - strlen(name) - 1, name);
    string_table[string_table_size - 1] = '\0';

    Elf_Data *data = elf_getdata(scn, NULL);
    if (data == NULL) {
        fprintf(stderr, "elf_getdata failed: %s\n", elf_errmsg(-1));
        return false;
    }

    uint8_t flags = 0;
    if (shdr.sh_flags & SHF_EXECINSTR) {
        flags |= ADI_FF_SEGMENT_FLAG_EXEC;
    }
    if (shdr.sh_flags & SHF_WRITE) {
        flags |= ADI_FF_SEGMENT_FLAG_WRITE;
    }
    if (shdr.sh_flags & SHF_ALLOC) {
        flags |= ADI_FF_SEGMENT_FLAG_READ;
    }
    if (data->d_buf != NULL) {
        flags |= ADI_FF_SEGMENT_FLAG_INFILE;
    }
    if(permissions & PERMS_PAGING){
        flags |= ADI_FF_SEGMENT_FLAG_PAGING;
    }
    if (permissions & PERMS_METADRIVER) {
        flags |= ADI_FF_SEGMENT_FLAG_META;
    }
    if(permissions & PERMS_USERSPACE){
        flags |= ADI_FF_SEGMENT_FLAG_USERSPACE;
    }

    adi_ff_segment_t seg = (adi_ff_segment_t){
        .name_offset = htole16(string_table_size - strlen(name) - 1),
        .flags = flags,
        .segment_offset = 0,
        .segment_size = htole32(shdr.sh_size),
        .virt_addr = htole64(shdr.sh_addr),
    };
    if(data->d_buf != NULL){
        seg.segment_offset = htole32(segment_contents_size);

        uint64_t fixed_size = ((shdr.sh_size / 4096) + 1) * 4096;

        segment_contents_size += fixed_size;
        segment_contents = (char*)realloc(segment_contents, segment_contents_size);
        if(segment_contents == NULL){
            printf("\e[1;31merror\e[0m: Failed to allocate memory\n");
            return false;
        }
        memset(segment_contents + segment_contents_size - fixed_size, 0, fixed_size);
        memcpy(segment_contents + segment_contents_size - fixed_size, data->d_buf, shdr.sh_size);
    }

    segment_table_size += sizeof(seg);
    segment_table = (char*)realloc(segment_table, segment_table_size);
    if(segment_table == NULL){
        printf("\e[1;31merror\e[0m: Failed to allocate memory\n");
        return false;
    }
    memcpy(segment_table + segment_table_size - sizeof(seg), &seg, sizeof(seg));

    return true;
}

bool prepare_segment_table(Elf* elf){
    segment_table_size = 0;

    size_t shstrndx;
    if (elf_getshdrstrndx(elf, &shstrndx) != 0) {
        fprintf(stderr, "elf_getshdrstrndx failed: %s\n", elf_errmsg(-1));
        return false;
    }

    // Iterate over all segments
    Elf_Scn *scn = NULL;
    int segment_index = 0;
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        GElf_Shdr shdr;
        if (gelf_getshdr(scn, &shdr) != &shdr) {
            fprintf(stderr, "gelf_getshdr failed: %s\n", elf_errmsg(-1));
            continue;
        }

        // Get segment name
        const char *segment_name = elf_strptr(elf, shstrndx, shdr.sh_name);
        if (!segment_name) {
            fprintf(stderr, "elf_strptr failed: %s\n", elf_errmsg(-1));
            segment_name = "<unknown>";
        }

        if(!append_segment(elf, scn, shstrndx)){
            return false;
        }
        segment_index++;
    }

    return true;
}

bool convert(char* driver_elf)
{
    bool result = true;
    if (elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "ELF library initialization failed: %s\n", elf_errmsg(-1));
        exit(1);
    }

    FILE* driver_elf_file = fopen(driver_elf, "r");
    if (driver_elf_file == NULL) {
        printf("\e[1;31merror\e[0m: Failed to open driver ELF file\n");
        goto error;
    }
    char* driver_adi = strdup(driver_elf); 
    if (driver_adi == NULL) {
        printf("\e[1;31merror\e[0m: Failed to allocate memory\n");
        goto error;
    }
    driver_adi[strlen(driver_adi) - 3] = '\0';
    strcat(driver_adi, "adi");

    FILE* driver_adi_file = fopen(driver_adi, "w+");
    if (driver_adi_file == NULL) {
        printf("\e[1;31merror\e[0m: Failed to create driver ADI file\n");
        goto error;
    }

    Elf *elf = elf_begin(fileno(driver_elf_file), ELF_C_READ, NULL);
    if (elf == NULL) {
        printf("\e[1;31merror\e[0m: Failed to parse ELF file\n");
        return false;
    }

    header.magic_num = ADI_FF_MAGIC;

    header.ver_major = ver_major;
    header.ver_minor = ver_minor;
    header.ver_patch = ver_patch;

    header.spec_version = htole16(adi_version);

    if(gelf_getclass(elf) == ELFCLASS64) {
        Elf64_Ehdr *ehdr = elf64_getehdr(elf);
        if(ehdr == NULL) {
            goto error;
        }
        header.entry_point = htole64(ehdr->e_entry);
    }else {
        printf("\e[1;31merror\e[0m: ELF class not supported(no 32-bit support rn)\n");
        goto error;
    }

    if(!prepare_string_table()){
        goto error;
    }
    if(!prepare_metalang_table(elf)) {
        goto error;
    }
    if(!prepare_segment_table(elf)){
        goto error;
    }

    header.string_table_offset = htole32(sizeof(header));
    header.metalang_table_offset = htole32(header.string_table_offset + string_table_size);
    header.segment_table_offset = htole32(header.metalang_table_offset + metalang_table_size);
    header.content_region_offset = htole32(header.segment_table_offset + segment_table_size);

    header.metalang_table_size = htole32(metalang_table_size);
    header.string_table_size = htole32(string_table_size);
    header.segment_table_size = htole32(segment_table_size);


    fwrite(&header, sizeof(header), 1, driver_adi_file);
    fwrite(string_table, string_table_size, 1, driver_adi_file);
    fwrite(metalang_table, metalang_table_size, 1, driver_adi_file);
    fwrite(segment_table, segment_table_size, 1, driver_adi_file);
    fwrite(segment_contents, segment_contents_size, 1, driver_adi_file);


    cleanup:
    elf_end(elf);
    fclose(driver_elf_file);
    fclose(driver_adi_file);
    free(driver_adi);
    free(metalang_table);
    free(segment_table);
    free(segment_contents);
    free(string_table);

    return result;
    error:

    result = false;
    goto cleanup;


}
