#include "server_parser.h"

void parser_init(parser_t *self){
    self->lower = 'A';
    self->upper = 'Z';
}

bool parser_is_valid(parser_t *self, char c){
    return ((self->lower <= c) && (self->upper >= c));
}

char* parser_parse_buffer(parser_t *self, 
                          const char *src_buffer,
                          size_t src_size){
    size_t parsable = 0;
    size_t i = 0;
    size_t j = 0;
    // el ultimo es el \n -> lo evito
    for (; i < src_size - 1; ++i){
        if (parser_is_valid(self, src_buffer[i])){
            ++parsable;
        }
    }

    // tengo que agregarle un \0 ya que desde afuera no se el tamaño
    // y pienso llamar a strlen
    char* new_string = (char*) calloc(parsable + 1, sizeof(char)); 
    if (!new_string)
        return NULL;

    for (i = 0; i < src_size - 1; ++i){
        if (parser_is_valid(self, src_buffer[i])){
            new_string[j++] = src_buffer[i];
        }
    }
    new_string[parsable] = 0;

    return new_string;
}

void parser_destroy(parser_t *self){}
