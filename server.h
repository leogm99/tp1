#ifndef __SERVER_H__
#define __SERVER_H__

#define ARGC_SERV 3
#define SERV argv[1]
#define KEY argv[2]

#include "common_socket.h"
#include "server_parser.h"
#include "common_mapper.h"
#include "server_cipher.h"


typedef struct {
    socket_t listener;
    socket_t serv_sock;
    
    parser_t parser;
    mapper_t mapper;

    cipher_t cipher;
} server_t;

int server_init(server_t *self, 
                 const char *service, const char *key);

char* server_prepare_message(server_t *self, char *received_buffer, 
                             size_t size, size_t* new_size,
                             char *empty_buffer_flag);

int server_send(server_t *self, char *buffer, size_t size);
char* server_receive(server_t *self, size_t *size_read);

char* server_parse(server_t *self, char *buffer, size_t size, size_t *new_size);
char* server_map(server_t *self, char *buffer, size_t size);

void server_destroy(server_t *self);

#endif
