#ifndef __FILE_READER_H__
#define __FILE_READER_H__

#define MAX_SIZE 180 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

typedef struct{
    FILE *file;
} file_reader_t;

int file_reader_init(file_reader_t *self, const char *filename);

size_t file_reader_readline(file_reader_t *self, char **buffer);

int file_reader_destroy(file_reader_t *self);

#endif
