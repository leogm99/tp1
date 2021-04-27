#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "client_file_reader.h"
#include "common_socket.h"

#define ARGC_CLIENT 4
#define HOST argv[1]
#define SERV argv[2]
#define FILE argv[3]



typedef struct {
    socket_t *cli_sock;
} client_t;


void client_init(client_t *self, socket_t *sock);

int client_connect(client_t *self, const char *host, const char *service);

int client_read_and_send(client_t *self, file_reader_t *file_reader, char *sent_less_flag);

int client_receive(client_t *self);

int main(int argc, const char *argv[]);

void client_destroy(client_t *self);

#endif
