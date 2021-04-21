#include "socket.h"

void socket_init(socket_t *self){}

void socket_destroy(socket_t *self){
    shutdown(self->fd, SHUT_RDWR);
    close(self->fd);
}

struct addrinfo *socket_getadrrinfo(socket_t *self, 
                                     const char *host,
                                     const char *service,
                                     const int caller_ai_flags){
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = caller_ai_flags;

    struct addrinfo *results;

    int addrinfo = getaddrinfo(host, service, &hints, &results);
    if (addrinfo != 0){
        fprintf(stderr, "getaddrinfo: failed");
        return NULL;
    }

    return results;
}

void socket_bind_and_listen(socket_t *self, 
                            const char *host, 
                            const char *service){
    struct addrinfo * results; 
    results = socket_getadrrinfo(self, host, service, AI_PASSIVE); // server

    if (!results){
        fprintf(stderr, "socket_getaddrinfo: got nullpointer");
        return;
    }

    self->fd = socket(results->ai_family, 
                      results->ai_socktype,
                      results->ai_protocol);

    if (self->fd < 0){
        fprintf(stderr, "socket_bind_and_listen: could not create socket");
        freeaddrinfo(results);
        return;
    }

    bind(self->fd, results->ai_addr, results->ai_addrlen);
    freeaddrinfo(results);

    listen(self->fd, 1);
}

void socket_accept(socket_t *listener, socket_t *peer){
}

void socket_connect(socket_t *self, const char *host, const char *service){
    struct addrinfo * results;
    results = socket_getadrrinfo(self, host, service, 0);

    if (!results){
        fprintf(stderr, "socket_getaddrinfo: got nullpointer");
        return;
    }

    self->fd = socket(results->ai_family, 
                      results->ai_socktype,
                      results->ai_protocol);

    if (self->fd < 0){
        fprintf(stderr, "socket_bind_and_listen: could not create socket");
        return;
    }

    connect(self->fd, results->ai_addr, results->ai_addrlen);
    freeaddrinfo(results);
}

ssize_t socket_send(socket_t *self, const char *buffer, size_t length){
    ssize_t send_ret, bytes_send; 
    send_ret = bytes_send = 0;

    while (bytes_send < length){
        send_ret = send(self->fd, buffer, length, MSG_NOSIGNAL);
        if (send_ret == -1){
            break;
        }
        bytes_send += send_ret;
    }
    return bytes_send;
}

ssize_t socket_receive(socket_t *self, char *buffer, size_t length){
    ssize_t recv_ret, bytes_recv;
    recv_ret = bytes_recv = 0;

    while (bytes_recv < length){
        recv_ret = recv(self->fd, buffer, length, MSG_NOSIGNAL);
        if (recv_ret == -1){
            break;
        }
        bytes_recv += recv_ret;
    }
    return bytes_recv;
}
