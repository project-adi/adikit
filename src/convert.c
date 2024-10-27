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
char* section_table = NULL;
uint32_t section_table_size = 0;

char* section_contents = NULL;
uint32_t section_contents_size = 0;

uintptr_t get_symbol_address(Elf* elf, char* symbol_name){
    size_t shstrndx;
    if (elf_getshdrstrndx(elf, &shstrndx) != 0) {
        fprintf(stderr, "ERROR: elf_getshdrstrndx failed: %s\n", elf_errmsg(-1));
        return 0;
    }

    // Iterate through sections to find the symbol table
    Elf_Scn *scn = NULL;
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        GElf_Shdr shdr;
        if (gelf_getshdr(scn, &shdr) != &shdr) {
            fprintf(stderr, "gelf_getshdr failed: %s\n", elf_errmsg(-1));
            continue;
        }

        // Check if the section is a symbol table
        if (shdr.sh_type == SHT_SYMTAB || shdr.sh_type == SHT_DYNSYM) {
            Elf_Data *data = elf_getdata(scn, NULL);
            if (!data) {
                fprintf(stderr, "elf_getdata failed: %s\n", elf_errmsg(-1));
                continue;
            }

            // Iterate through symbols
            size_t num_symbols = shdr.sh_size / shdr.sh_entsize;
            for (size_t i = 0; i < num_symbols; ++i) {
                GElf_Sym sym;
                if (gelf_getsym(data, i, &sym) != &sym) {
                    fprintf(stderr, "gelf_getsym failed: %s\n", elf_errmsg(-1));
                    continue;
                }

                const char *name = elf_strptr(elf, shdr.sh_link, sym.st_name);
                if (name && strcmp(name, symbol_name) == 0) {
                    return (uintptr_t)sym.st_value;
                }
            }
        }
    }

    printf("Symbol '%s' not found.\n", symbol_name);
    return 0;
}

bool prepare_string_table(){
    //TODO: use the string table for more
    string_table_size = strlen(name) + strlen(author) + 2;
    string_table = (char*) malloc(string_table_size);
    memset(string_table, 0, string_table_size);
    strcpy(string_table, name);
    string_table[strlen(name)] = '\0';
    strcpy(string_table + strlen(name) + 1, author);
    string_table[string_table_size] = '\0';

    header.name_offset = 0;
    header.author_offset = htole16(strlen(name) + 1);
    header.string_table_offset = htole32(sizeof(header));

    return true;
}

bool append_metalang(Elf* elf, char* metalang){
    adi_ff_metalang_t mlang = (adi_ff_metalang_t){
        .id = htole32(get_metalang_id(metalang)),
        .pointer_addr = htole64(get_symbol_address(elf, metalang)),
    };

    memcpy(metalang_table + metalang_table_size - sizeof(mlang), &mlang, sizeof(mlang));

    metalang_table_size += sizeof(mlang);
    metalang_table = (char*)realloc(metalang_table, metalang_table_size);
    if(metalang_table == NULL){
        printf("ERROR: Failed to allocate memory\n");
        return false;
    }

    return true;
}

bool prepare_metalang_table(Elf* elf){
    metalang_table_size = sizeof(adi_ff_metalang_t);
    metalang_table = (char*)malloc(metalang_table_size);

    for(int i = 0; metalangs_used[i]; i++){
        append_metalang(elf,metalangs_used[i]);
    }

    for(int i = 0; metalangs_implemented[i]; i++){
        append_metalang(elf,metalangs_implemented[i]);
    }

    return true;
}

bool append_section(Elf* elf, Elf_Scn* scn,size_t shstrndx){
    GElf_Shdr shdr;
    if (gelf_getshdr(scn, &shdr) != &shdr) {
        fprintf(stderr, "gelf_getshdr failed: %s\n", elf_errmsg(-1));
        return false;
    }

    //get the section name
    const char *name = elf_strptr(elf, shstrndx, shdr.sh_name);

    if(strcmp(name, ".comment") == 0 || strcmp(name, ".strtab") == 0 || strcmp(name, ".shstrtab") == 0){
        return true;
    }

    string_table_size += strlen(name) + 1;
    string_table = (char*)realloc(string_table, string_table_size);
    if(string_table == NULL){
        printf("ERROR: Failed to allocate memory\n");
        return false;
    }
    strcpy(string_table + string_table_size - strlen(name) - 1, name);
    string_table[string_table_size - 1] = '\0';

    header.metalang_table_offset = htole32(le32toh(header.metalang_table_offset) + strlen(name) + 1);
    header.segment_table_offset = htole32(le32toh(header.segment_table_offset) + strlen(name) + 1);

    Elf_Data *data = elf_getdata(scn, NULL);
    if (data == NULL) {
        fprintf(stderr, "elf_getdata failed: %s\n", elf_errmsg(-1));
        return false;
    }

    uint8_t flags = 0;
    if (shdr.sh_flags & SHF_EXECINSTR) {
        flags |= ADI_FF_SECTION_FLAG_EXEC;
    }
    if (shdr.sh_flags & SHF_WRITE) {
        flags |= ADI_FF_SECTION_FLAG_WRITE;
    }
    if (shdr.sh_flags & SHF_ALLOC) {
        flags |= ADI_FF_SECTION_FLAG_READ;
    }
    if (data->d_buf != NULL) {
        flags |= ADI_FF_SECTION_FLAG_INFILE;
    }
    if(permissions & PERMS_PAGING){
        flags |= ADI_FF_SECTION_FLAG_PAGING;
    }
    if (permissions & PERMS_METADRIVER) {
        flags |= ADI_FF_SECTION_FLAG_META;
    }
    if(permissions & PERMS_USERSPACE){
        flags |= ADI_FF_SECTION_FLAG_USERSPACE;
    }

    adi_ff_segment_t seg = (adi_ff_segment_t){
        .name_offset = htole16(string_table_size - strlen(name) - 1),
        .flags = flags,
        .segment_offset = 0,
        .segment_size = htole32(shdr.sh_size),
    };
    if(data->d_buf != NULL){
        seg.segment_offset = htole32(section_contents_size);

        section_contents_size += shdr.sh_size;
        section_contents = (char*)realloc(section_contents, section_contents_size);
        if(section_contents == NULL){
            printf("ERROR: Failed to allocate memory\n");
            return false;
        }
        memcpy(section_contents + section_contents_size - shdr.sh_size, data->d_buf, shdr.sh_size);
    }

    printf("Section: %s\n", name);

    return true;
}

bool prepare_section_table(Elf* elf){

    header.segment_table_offset = header.metalang_table_offset + metalang_table_size;
    section_table_size = 0;

    size_t shstrndx;
    if (elf_getshdrstrndx(elf, &shstrndx) != 0) {
        fprintf(stderr, "elf_getshdrstrndx failed: %s\n", elf_errmsg(-1));
        return false;
    }

    // Iterate over all sections
    Elf_Scn *scn = NULL;
    int section_index = 0;
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        GElf_Shdr shdr;
        if (gelf_getshdr(scn, &shdr) != &shdr) {
            fprintf(stderr, "gelf_getshdr failed: %s\n", elf_errmsg(-1));
            continue;
        }

        // Get section name
        const char *section_name = elf_strptr(elf, shstrndx, shdr.sh_name);
        if (!section_name) {
            fprintf(stderr, "elf_strptr failed: %s\n", elf_errmsg(-1));
            section_name = "<unknown>";
        }

        if(!append_section(elf, scn, shstrndx)){
            return false;
        }
        section_index++;
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
        printf("ERROR: Failed to open driver ELF file\n");
        goto error;
    }
    char* driver_adi = strdup(driver_elf); 
    if (driver_adi == NULL) {
        printf("ERROR: Failed to allocate memory\n");
        goto error;
    }
    driver_adi[strlen(driver_adi) - 3] = '\0';
    strcat(driver_adi, "adi");

    FILE* driver_adi_file = fopen(driver_adi, "w+");
    if (driver_adi_file == NULL) {
        printf("ERROR: Failed to create driver ADI file\n");
        goto error;
    }

    Elf *elf = elf_begin(fileno(driver_elf_file), ELF_C_READ, NULL);
    if (elf == NULL) {
        printf("ERROR: Failed to parse ELF file\n");
        return false;
    }

    header.magic_num = ADI_FF_MAGIC;

    header.version = (typeof(header.version)) {
        .major = ver_major,
        .minor = ver_minor,
        .patch = ver_patch
    };

    header.cfr_addr = htole64(get_symbol_address(elf, "core"));

    if(gelf_getclass(elf) == ELFCLASS32) {
        Elf32_Ehdr *ehdr = elf32_getehdr(elf);
        if(ehdr == NULL) {
            goto error;
        }
        header.entry_point = htole32(ehdr->e_entry);
        header.ISA = htole16(ehdr->e_machine);
    }else if(gelf_getclass(elf) == ELFCLASS64) {
        Elf64_Ehdr *ehdr = elf64_getehdr(elf);
        if(ehdr == NULL) {
            goto error;
        }
        header.entry_point = htole64(ehdr->e_entry);
        header.ISA = htole16(ehdr->e_machine);
    }else {
        printf("ERROR: ELF class not supported\n");
        goto error;
    }

    if(!prepare_string_table()){
        goto error;
    }
    if(!prepare_metalang_table(elf)) {
        goto error;
    }
    if(!prepare_section_table(elf)){
        goto error;
    }


    fwrite(&header, sizeof(header), 1, driver_adi_file);
    fwrite(string_table, string_table_size, 1, driver_adi_file);
    fwrite(metalang_table, metalang_table_size, 1, driver_adi_file);
    fwrite(section_table, section_table_size, 1, driver_adi_file);
    fwrite(section_contents, section_contents_size, 1, driver_adi_file);


    cleanup:
    elf_end(elf);
    fclose(driver_elf_file);
    fclose(driver_adi_file);
    free(driver_adi);

    return result;
    error:

    result = false;
    goto cleanup;


}
