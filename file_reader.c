#include "file_reader.h"

int file_reader_init(file_reader_t *self, const char *filename){
    if(!filename){
        return -1;
    }
    char c = '-';
    self->file = (strcmp(filename, &c) == 0) ? stdin : fopen(filename, "r");
    if (!self->file){
        return -1;
    }
    return 0;
}

unsigned short file_reader_readline(file_reader_t *self, char *buffer){
    if (fgets(buffer, MAX_SIZE, self->file) != NULL){
        return strlen(buffer);
    }
    return 0;
}

int file_reader_destroy(file_reader_t *self){
    if (self->file != stdin){
        fclose(self->file);
    }
    return 0;
}
