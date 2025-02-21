#include "create.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

bool create() {
    char *driver_name = malloc(256);
    char *author = malloc(256);
    char *directory = malloc(256);

    if (!driver_name || !author || !directory) {
        printf("\e[1;31merror\e[0m: Memory allocation failed\n");
        free(driver_name);
        free(author);
        free(directory);
        return 1;
    }
            
    printf("➜  Enter the name of the driver: ");
    fgets(driver_name, 256, stdin);
    driver_name[strcspn(driver_name, "\n")] = 0;
            
    printf("➜  Enter the authors name: ");
    fgets(author, 256, stdin);
    author[strcspn(author, "\n")] = 0;
            
    printf("➜  Enter directory name (default: %s/): ", driver_name);
    fgets(directory, 256, stdin);
    directory[strcspn(directory, "\n")] = 0;
            
    if (strlen(directory) == 0) {
        strcpy(directory, driver_name);
    }

    if (!directory || !driver_name || !author) {
        free(driver_name);
        free(author);
        free(directory);
        return false;
    }
    
    // Create the main directory
    if (mkdir(directory, 0777) != 0) {
        free(driver_name);
        free(author);
        free(directory);
        return false;
    }
    
    // Create the src directory path
    size_t dir_len = strlen(directory);
    char* src_path = malloc(dir_len + 5); // "src/" + null terminator
    if (!src_path) {
        free(driver_name);
        free(author);
        free(directory);
        return false;
    }
    
    sprintf(src_path, "%s/src", directory);
    if (mkdir(src_path, 0777) != 0) {
        free(src_path);
        free(driver_name);
        free(author);
        free(directory);
        return false;
    }
    
    // Create the main.c file path
    char* main_path = malloc(1024); // "src/main.c" + null terminator
    if (!main_path) {
        free(src_path);
        free(driver_name);
        free(author);
        free(directory);
        return false;
    }
    
    sprintf(main_path, "%s/main.c", src_path);
    FILE* file = fopen(main_path, "w+");
    if (!file) {
        printf("\e[1;31merror\e[0m: Failed to create file: %s\n", main_path);
        free(src_path);
        free(main_path);
        free(driver_name);
        free(author);
        free(directory);
        return false;
    }

    fprintf(file,
        "#include <stdint.h>\n"
        "#include <stdbool.h>\n"
        "#include <core.h>\n"
        "#include <metalanguages/misc/storage.h>\n\n"
        "adi_core_t* core;\n\n"
        "metalang_storage_t* misc_storage;\n"
        "uint32_t device_id = 0;\n\n"
        "sdev_ident_t storage_ident_callback(adi_device_t* dev) {\n"
        "\treturn (sdev_ident_t){\n"
        "\t\t.size = sizeof(\"Hello, world!\"),\n"
        "\t\t.sector_size = 1,\n"
        "\t\t.read_only = true\n"
        "\t};\n"
        "}\n\n"
        "uint32_t storage_transact_callback(adi_device_t* dev, bool write, uint32_t offset, uint32_t count, void* buffer) {\n"
        "\tif (write) return 0;\n"
        "\tif (offset + count > sizeof(\"Hello, world!\")) return 0;\n\n"
        "\tchar* hw = \"Hello, world!\";\n"
        "\tcore->memcpy(buffer, hw + offset, count);\n\n"
        "\tmisc_storage->signal_transaction_done(dev->metalangs_implemented[0]->params, 1);\n"
        "\treturn 1;\n"
        "}\n\n"
        "int _start() {\n"
        "\tmetalanguage_t langs[] = {misc_storage->new(storage_ident_callback, storage_transact_callback)};\n"
        "\tdevice_id = core->register_device(langs, 1);\n"
        "\treturn 0;\n"
        "}\n"
    );
    fclose(file);
    
    // Create the .drvdesc file path
    char* drvdesc_path = malloc(dir_len + 10); // "/.drvdesc" + null terminator
    if (!drvdesc_path) {
        free(src_path);
        free(main_path);
        free(driver_name);
        free(author);
        free(directory);
        return false;
    }
    
    sprintf(drvdesc_path, "%s/.drvdesc", directory);
    FILE* drvdesc_file = fopen(drvdesc_path, "w+");
    if (!file) {
        printf("\e[1;31merror\e[0m: Failed to create file: %s\n", drvdesc_path);
        free(src_path);
        free(main_path);
        free(drvdesc_path);
        free(driver_name);
        free(author);
        free(directory);
        return false;
    }

    fprintf(drvdesc_file,
        "name: %s\n"
        "author: %s\n"
        "#commented out stuff below is default\n"
        "#cross-arch: x86\n"
        "#permissions: none\n"
        "#require_kernel: false\n"
        "src_dir: src\n"
        "implements:\n"
        "\t-misc_storage\n",
        driver_name, author
    );

    fclose(drvdesc_file);

    char linker[256];
    strcpy(linker, directory);
    strcat(linker, "/linker.ld");
    FILE* linker_file = fopen(linker, "w+");
    if(file == NULL) {
        printf("\e[1;31merror\e[0m: Failed to create file: %s\n", linker);
        return false;
    }
    fprintf(linker_file, 
        "OUTPUT_FORMAT(\"elf64-x86-64\")\n"
        "ENTRY(_start)\n\n"
        "SECTIONS\n"
        "{\n"
        "    . = ALIGN(CONSTANT(MAXPAGESIZE));\n\n"
        "    .text : {\n"
        "        *(.text)\n"
        "    }\n"
        "    . = ALIGN(CONSTANT(MAXPAGESIZE));\n\n"
        "    .data : {\n"
        "        *(.data)\n"
        "    }\n"
        "    . = ALIGN(CONSTANT(MAXPAGESIZE));\n\n"
        "    .rodata : {\n"
        "        *(.rodata)\n"
        "    }\n\n"
        "    . = ALIGN(CONSTANT(MAXPAGESIZE));\n"
        "    .bss : {\n"
        "        *(.bss)\n"
        "    }\n\n"
        "    /DISCARD/ : {\n"
        "        *(.comment)\n"
        "        *(.note*)\n"
        "        *(.eh_frame*)\n"
        "        *(.eh_frame_hdr)\n"
        "        *(.got)\n"
        "        *(.plt)\n"
        "        *(.dyn*)\n"
        "        *(.gnu*)\n"
        "        *(.interp)\n"
        "        *(.hash)\n"
        "        *(.rel*)\n"
        "    }\n"
        "}\n");        

    printf("Created new driver in %s\n", directory);

    fclose(linker_file);
    
    // Cleanup
    free(src_path);
    free(main_path);
    free(drvdesc_path);

    free(driver_name);
    free(author);
    free(directory);
    return true;
}