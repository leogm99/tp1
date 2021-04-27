#include "file_reader.h"

int file_reader_init(file_reader_t *self, const char *filename){
    if (!filename){
        return -1;
    }
    char c = '-';
    self->file = (strcmp(filename, &c) == 0) ? stdin : fopen(filename, "r");
    if (!self->file){
        return -1;
    }
    return 0;
}

size_t file_reader_readline(file_reader_t *self, char **buffer){
    size_t n = 0;
    ssize_t ret = getline(buffer, &n, self->file);
    if (ret == -1){
        return 0;
    }
    return ret;
}

int file_reader_destroy(file_reader_t *self){
    if (self->file != stdin){
        fclose(self->file);
    }
    return 0;
}
