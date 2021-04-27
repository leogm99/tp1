#ifndef __MAPPER_H__
#define __MAPPER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct{
    unsigned short offset;
    bool to_letter;
} mapper_t;

void mapper_init(mapper_t *self);

void map(mapper_t *self, 
         const void *parsed_buffer, 
         void *dest_buffer, size_t size);

void map_to_int(mapper_t *self, 
                const char *parsed_buffer, 
                short *dest_buffer, size_t dest_size);

void map_to_text(mapper_t *self, 
                 const short *parsed_buffer, 
                 char *dest_buffer, size_t dest_size);

void mapper_invert(mapper_t *self);

void mapper_destroy(mapper_t *self);

#endif
