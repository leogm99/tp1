#ifndef __SERVER_H__
#define __SERVER_H__


#define SERV argv[1]
#define KEY argv[2]

#include "socket.h"
#include "parser.h"
#include "mapper.h"
#include "cipher.h"


typedef struct {
    socket_t listener;
    socket_t serv_sock;
    
    parser_t parser;
    mapper_t mapper;

    cipher_t cipher;
} server_t;

void server_init(server_t *self);

void server_create_key(server_t *self, const char *raw_key);

void server_bind_and_listen(server_t *self, const char *service);
int server_accept(server_t *self);

int server_send(server_t *self, char *buffer, size_t size);
char* server_receive(server_t *self, size_t *size_read);

char* server_parse(server_t *self, char *buffer, size_t size, size_t *new_size);
short* server_map(server_t *self, char *buffer, size_t size);

short* server_encode(server_t *self, short *buffer, const size_t size, size_t *new_size);

char *server_recast(server_t *self, short *buffer, size_t size);

void server_destroy(server_t *self);

#endif
