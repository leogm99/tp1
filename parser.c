#include "parser.h"

void parser_create(parser_t *self){
    self->lower = 'A';
    self->upper = 'Z';
}

bool parser_is_valid(parser_t *self, char c){
    return ((self->lower <= c) && (self->upper >= c));
}

char* parser_parse_string(parser_t *self, 
                         char *src_buffer,
                         size_t src_size){
    size_t parsable = 0;
    size_t i = 0;
    size_t j = 0;
    for (; i < src_size - 1; ++i){
        if (parser_is_valid(self, src_buffer[i])){
            ++parsable;
        }
    }

    char* new_string = (char*) calloc(parsable + 1, sizeof(char));
    if (!new_string)
        return NULL;

    for(i = 0; i < src_size - 1; ++i){
        if (parser_is_valid(self, src_buffer[i])){
            new_string[j++] = src_buffer[i];
        }
    }
    new_string[parsable] = 0;

    return new_string;
}

void parser_destroy(parser_t *self){}
