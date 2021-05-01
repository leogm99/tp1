#ifndef __MAPPER_H__
#define __MAPPER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct{
    char offset;
    bool to_letter;
} mapper_t;

void mapper_init(mapper_t *self);

void mapper_map(mapper_t *self, 
         const void *parsed_buffer, 
         void *dest_buffer, size_t size);

void mapper_to_number(mapper_t *self, 
                const char *parsed_buffer, 
                short *dest_buffer, size_t dest_size);

void mapper_to_text(mapper_t *self, 
                 const char *parsed_buffer, 
                 char *dest_buffer, size_t dest_size);

void mapper_invert(mapper_t *self);

void mapper_destroy(mapper_t *self);

#endif
