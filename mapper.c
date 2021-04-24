#include "mapper.h"

void mapper_init(mapper_t *self){
    self->offset = 'A';
    self->to_letter = false;
}

void map(mapper_t *self, const void *parsed_buffer, 
         void *dest_buffer, size_t size){
    self->to_letter ? map_to_text(self, parsed_buffer, dest_buffer, size) :
                      map_to_int(self, parsed_buffer, dest_buffer, size);
}

void map_to_int(mapper_t *self, 
                const char *parsed_buffer,
                char *dest_buffer,
                size_t dest_size){
    for (size_t i = 0; i < dest_size; ++i){
        dest_buffer[i] = parsed_buffer[i] - self->offset;
    }
}

void map_to_text(mapper_t *self,
                 const *parsed_buffer,
                 char *dest_buffer,
                 size_t dest_size){
    for (size_t i = 0; i < dest_size; ++i){
        dest_buffer[i] = parsed_buffer[i] + self->offset;
    }
    dest_buffer[dest_size - 1] = 0;
}

void mapper_invert(mapper_t *self){
    self->to_letter = !self->to_letter;
}

void mapper_destroy(mapper_t *self){}
