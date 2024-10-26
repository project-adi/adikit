#include "create.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

bool create(char* directory) {
    if (directory == NULL) {
        return false;
    }

    //make the directory
    if (mkdir(directory, 0777) != 0) {
        return false;
    }

    //make the src directory
    char src[1024];
    strcpy(src, directory);
    strcat(src, "/src");
    if (mkdir(src, 0777) != 0) {
        return false;
    }

    char* main = strcat(src, "/main.c");
    FILE* file = fopen(main, "w+");
    if(file == NULL) {
        printf("Failed to create file: %s\n", main);
        return false;
    }

    fprintf(file,"#include <core.h>\n");
    fprintf(file,"#include <metalanguages/misc/storage.h>\n");
    fprintf(file,"\n");
    fprintf(file,"metalang_storage_t* storage;\n");
    fprintf(file,"uint32_t devide_id = 0;\n");
    fprintf(file,"\n");
    fprintf(file,"sdev_ident_t storage_ident_callback(adi_device_t* dev) {\n");
    fprintf(file,"	return {\n");
    fprintf(file,"		.size = sizeof(\"Hello, world!\"),\n");
    fprintf(file,"		.sector_size = 1,\n");
    fprintf(file,"		.read_only = true\n");
    fprintf(file,"	};\n");
    fprintf(file,"}\n");
    fprintf(file,"\n");
    fprintf(file,"uint32_t storage_transact_callback(bool write,uint32_t offset,uint32_t count,void* buffer){\n");
    fprintf(file,"\n");
    fprintf(file,"	if(write)\n");
    fprintf(file,"		return 0;\n");
    fprintf(file,"	if (offset + count > sizeof(\"Hello, world!\"))\n");
    fprintf(file,"		return 0;\n");
    fprintf(file,"\n");
    fprintf(file,"	char* hw = \"Hello, world!\";\n");
    fprintf(file,"\n");
    fprintf(file,"	memcpy(buffer, hw + offset,count);\n");
    fprintf(file,"\n");
    fprintf(file,"	// recieving the transaction_done event before the storage_transact call finishing is a perfectly normal scenario\n");
    fprintf(file,"	// and programs should account for it\n");
    fprintf(file,"	storage->signal_transaction_done(device_id,1);\n");
    fprintf(file,"\n");
    fprintf(file,"	return 1;\n");
    fprintf(file,"}\n");
    fprintf(file,"\n");
    fprintf(file,"int _init() {\n");
    fprintf(file,"	metalanguage_t langs[] = {storage->new(storage_ident_callback,storage_transact_callback)};\n");
    fprintf(file,"	device_id = register_devices(storage,1);\n");
    fprintf(file,"	return 0;\n");
    fprintf(file,"}\n");

    fclose(file);

    //create a file in directory named "drvdesc"
    char* drvdesc = strcat(directory, "/.drvdesc");
    file = fopen(drvdesc, "w+");
    if(file == NULL) {
        printf("Failed to create file: %s\n", drvdesc);
        return false;
    }
    fprintf(file,"name: Lonely Driver\n");
    fprintf(file,"author: Lolguy91\n");
    fprintf(file,"#commented out stuff bellow is default\n");
    fprintf(file,"#cross-arch: x86\n");
    fprintf(file,"#permissions: none\n");
    fprintf(file,"#require_kernel: false\n");
    fprintf(file,"src_dir: src\n");
    fprintf(file,"implements:\n");
    fprintf(file,"\t-misc_storage\n");
    fclose(file);

    return true;
}