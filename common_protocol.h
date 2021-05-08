#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "common_socket.h"
#include <stdlib.h>

typedef struct{
    socket_t* cli_sock;
    socket_t* serv_sock;
} protocol_t;

typedef uint16_t (*Endianess)(uint16_t);

int protocol_server_init(protocol_t *self, const char *serv);
int protocol_client_init(protocol_t *self, const char *host, const char *serv);

int protocol_send(protocol_t *self, char *buffer, 
                      size_t size, Endianess end_func);

char* protocol_receive(protocol_t *self, size_t *size_read,
                       Endianess end_func, char *recv_zero_flag);


int protocol_destroy(protocol_t *self);

#endif 
