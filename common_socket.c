#include "common_socket.h"

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

    struct addrinfo * aux = results;
    for (; aux; aux = aux->ai_next){
        self->fd = socket(aux->ai_family, 
                          aux->ai_socktype,
                          aux->ai_protocol);
        bind(self->fd, aux->ai_addr, aux->ai_addrlen);
    }


    if (self->fd < 0){
        fprintf(stderr, "socket_bind_and_listen: could not create socket");
        freeaddrinfo(results);
        return;
    }

    freeaddrinfo(results);

    listen(self->fd, 1);
}

int socket_accept(socket_t *listener, socket_t *peer){
    return accept(listener->fd, 0, 0);
}

int socket_connect(socket_t *self, const char *host, const char *service){
    struct addrinfo * results;
    results = socket_getadrrinfo(self, host, service, 0);

    if (!results){
        printf("%s\n", "socket_getaddrinfo: got nullpointer");
        return -1;
    }

    struct addrinfo *aux = results;

    for (; aux; aux = aux->ai_next){
        self->fd = socket(aux->ai_family, 
                          aux->ai_socktype,
                          aux->ai_protocol);
        if (connect(self->fd, aux->ai_addr, aux->ai_addrlen) == -1){
            // si no me pude conectar tengo que devolver -1
            break;
        }
    }

    if (self->fd < 0){
        printf("%s\n", "socket_bind_and_listen: could not create socket");
        freeaddrinfo(results);
        return -1;
    }


    freeaddrinfo(results);
    return 0;
}

ssize_t socket_send(socket_t *self, const void *buffer, size_t length){
    ssize_t bytes_send = 0;
    const char *aux = buffer;

    while (bytes_send < length){
        ssize_t send_ret = send(self->fd, &aux[bytes_send], 
                        length - bytes_send, MSG_NOSIGNAL);
        if (send_ret == -1){
            break;
        }
        bytes_send += send_ret;
    }

    return bytes_send;
}

ssize_t socket_receive(socket_t *self, void *buffer, size_t length){
    ssize_t bytes_recv = 0;
    char *aux = buffer;

    while (bytes_recv < length){
        ssize_t recv_ret = recv(self->fd, &aux[bytes_recv], 
                                length - bytes_recv, 0);
        if (recv_ret == -1 || recv_ret == 0){
            break;
        }
        bytes_recv += recv_ret;
    }
    return bytes_recv;
}
