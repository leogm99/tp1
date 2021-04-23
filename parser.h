#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    char lower;
    char upper;
} parser_t;


void parser_init(parser_t *self);

bool parser_is_valid(parser_t *self, char c);

char* parser_parse_string(parser_t *self, const char *src_buffer, size_t src_size);

void parser_destroy(parser_t *self);

#endif
