#ifndef __CIPHER_H__
#define __CIPHER_H__

// Prefiero mantenerlo en el stack
// La cota no es arbitraria ya que por enunciado
// la key mas larga puede tener 16 caracteres
#define KEY_MAX 16

#include "common_mapper.h"
#include "math.h"


typedef struct {
    size_t dim;
    short key[KEY_MAX];
} cipher_t;


void cipher_init(cipher_t *self);

void cipher_create_key(cipher_t *self, mapper_t *mapper, const char *raw_key);

short* cipher_encode(cipher_t *self, short *buffer, const size_t buffer_size, size_t *new_size);

short* cipher_resize(cipher_t *self, short *buffer, const size_t old_size, size_t *resize);

void cipher_destroy(cipher_t *self);

#endif
