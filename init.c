#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include "structure.h"

struct CONFIG
{
    char * CONF_NAME;
    u_int16_t PAGE_SIZE;
    u_int16_t HEADER_SIZE;
};
char* create_full_path(const char* root, const char * subfolder[], int size) {

    
    int cur_max = 20 * sizeof(char);

    int cur_size = strlen(root) + strlen(subfolder[0]) + 1;

    char* full_dir = malloc(cur_max);

    snprintf(full_dir, sizeof(root), "%s", root);
    for(int i = 0; i < size; i++) {

        cur_size += strlen(subfolder[i]) + 2; // 1 for the "/" symbol 1 for the \0

        if(cur_size == cur_max) {

            cur_max += strlen(subfolder[1]) * sizeof(char);

            full_dir = (char *)realloc(full_dir ,cur_max);
        }
        snprintf(full_dir + strlen(full_dir), strlen(subfolder[i]) + 2, "/%s", subfolder[i]);
    }
    return full_dir;

}


void create_dir(const char* db_path, const char* subfolder) {

    const char* subfolders = { subfolder };

    char* table_dir = create_full_path(db_path, &subfolders, 1);

    struct stat st = {0};

    if(stat(db_path , &st) != 0){
        perror("stat");
        exit(-1);
    };
    if(mkdir(table_dir, 0777) != 0) {
        perror("Are you sure you have not initialized databse in this folder already?");
        exit(-1);
    };

    free(table_dir);

}
void create_conf_file(const char* db_path, struct CONFIG conf) {

    const char ** subfolders = malloc(2 * sizeof(char*));

    subfolders[0] = CONF_FOLDER;
    subfolders[1] = conf.CONF_NAME;


    char* conf_file_dir = create_full_path( db_path, subfolders, 2);
    FILE *fPtr = fopen(conf_file_dir, "w");
    
    /* fopen() return NULL if last operation was unsuccessful */
    if(fPtr == NULL)
    {
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }
    fputs("PAGE_SIZE=", fPtr);
    fprintf(fPtr, "%d", conf.PAGE_SIZE);
    fputs("\n", fPtr);

    fputs("HEADER_SIZE=", fPtr);
    fprintf(fPtr, "%d", conf.HEADER_SIZE);
    fputs("\n", fPtr);

    fclose(fPtr);

    free(conf_file_dir);
    free(subfolders);
}
int main(int argc, const char* argv[]) {
    
    const char * db_name = argv[1];
    
    if (!db_name) {
        printf("No path given!\n");
        exit(1);
    }
    create_dir(db_name, TABLE_FOLDER);
    create_dir(db_name, INDEX_FOLDER);
    create_dir(db_name, CONF_FOLDER);
    create_dir(db_name, FMS_FOLDER);


    struct CONFIG conf = {.CONF_NAME = "db.conf", .PAGE_SIZE = 8192, .HEADER_SIZE = 24 };

    create_conf_file(db_name, conf);

    printf("Db initialized correctly!'\n");
}
