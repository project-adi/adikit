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

    fprintf(file, "#include <stdio.h>\n\nint main(int argc, char **argv) {\n");
    fprintf(file, "\tprintf(\"Hello World!\\n\");\n");
    fprintf(file, "\treturn 0;\n");
    fprintf(file, "}\n");

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
    fprintf(file,"#cross-arch: aarch64\n");
    fprintf(file,"regions:\n");
    fprintf(file,"\tmain:\n");
    fprintf(file,"\t\tpermissions: low\n");
    fprintf(file,"\trequire_kernel: false\n");
    fprintf(file,"\tsrc_dir: src\n");
    fprintf(file,"\tuses:\n");
    fprintf(file,"\t\t\t- x86\n");
    fprintf(file,"\t\timplements:\n");
    fprintf(file,"\t\t\t- timekeeper");
    fclose(file);

    return true;
}