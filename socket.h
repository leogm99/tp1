#define _POSIX_C_SOURCE 200809L
#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

typedef struct{
    int fd;
} socket_t;

void socket_init(socket_t *self);
void socket_destroy(socket_t *self);

struct addrinfo *socket_getadrrinfo(socket_t *self, 
                                     const char *host,
                                     const char *service,
                                     const int caller_ai_flags);

void socket_bind_and_listen(socket_t *self,
                            const char *host,
                            const char *service);
void socket_accept(socket_t *listener, socket_t *peer);

void socket_connect(socket_t *self, const char *host, const char *service);

ssize_t socket_send(socket_t *self, const char *buffer, size_t length);
ssize_t socket_receive(socket_t *self, char *buffer, size_t length);

#endif
