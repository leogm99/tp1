#include "common_mapper.h"

void mapper_init(mapper_t *self){
    self->offset = 'A';
    self->to_letter = false;
}

void mapper_map(mapper_t *self, const void *parsed_buffer, 
         void *dest_buffer, size_t size){
    self->to_letter ? mapper_to_text(self, parsed_buffer, dest_buffer, size) :
                      mapper_to_number(self, parsed_buffer, dest_buffer, size);
}

void mapper_to_number(mapper_t *self, 
                const char *parsed_buffer,
                char *dest_buffer,
                size_t dest_size){
    for (size_t i = 0; i < dest_size; ++i){
        dest_buffer[i] = parsed_buffer[i] - self->offset;
    }
}

void mapper_to_text(mapper_t *self,
                 const char *parsed_buffer,
                 char *dest_buffer,
                 size_t dest_size){
    for (size_t i = 0; i < dest_size; ++i){
        dest_buffer[i] = parsed_buffer[i] + self->offset;
    }
}

void mapper_invert(mapper_t *self){
    self->to_letter = !self->to_letter;
}

void mapper_destroy(mapper_t *self){}
