#include "server_cipher.h"

void cipher_init(cipher_t *self){}

// Por mas de que el string tenga 16 c + \0
// No interesa, ya que lo unico que necesito 
// son los numeros (hasta 16)
int cipher_create_key(cipher_t *self, mapper_t *mapper, const char *raw_key){
    switch (strlen(raw_key)){
        case 4:
            self->dim = 2;
            break;
        case 9:
            self->dim = 3;
            break;
        case 16:
            self->dim = 4;
            break;
        default:
            return -1;
    }
    map(mapper, raw_key, self->key, KEY_MAX);
    return 0;
}

// La parte logica, debe multiplicar la key con el buffer
// Hay que lograr un toque de aritmetica modular
// Trabajo con short's ya que quiero evitar desbordamiento de chars
short* cipher_encode(cipher_t *self, short *buffer,
                     const size_t buffer_size, size_t *new_size){
    size_t size = buffer_size;
    size_t key_size = self->dim * self->dim;
    short *aux = buffer;
    bool resized = false;

    if (buffer_size % self->dim){
        if (!(aux = cipher_resize(self, buffer, buffer_size, &size))){
            return NULL;
        }
        resized = true;
        *new_size = size;
    }

    short *new_buf = calloc(size, sizeof(short));
    if (!new_buf){
        if (resized){
            free(aux);
        }
        return NULL;
    }

    for (size_t i = 0; i < size - self->dim + 1; i+=self->dim){
        for (size_t j = 0; j < key_size; ++j){
            new_buf[i + (j / self->dim)] += aux[i + (j % self->dim)] 
                                            * self->key[j];
        }
    }

    for (size_t i = 0; i < size; ++i){
        new_buf[i] %= 26;
    }

    if (resized){
        free(aux);
    }

    return new_buf;
}

short* cipher_resize(cipher_t *self, short *buffer, 
                     const size_t old_size, size_t *resize){
    size_t new_size = old_size + self->dim - (old_size % self->dim);

    //short* new_buffer = realloc(buffer, sizeof(short) * new_size);
    short *new_buffer = calloc(new_size, sizeof(short));
    if (!new_buffer){
        return NULL;
    }

    memcpy(new_buffer, buffer, sizeof(short) * old_size);

    *resize = new_size;
    return new_buffer;
}

void cipher_destroy(cipher_t *self){}
