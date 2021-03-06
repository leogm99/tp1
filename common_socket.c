#include "common_socket.h"

void socket_init(socket_t *self){
    self->fd = -1;
}

void socket_destroy(socket_t *self){
    if (self->fd != -1){
        shutdown(self->fd, SHUT_RDWR);
        close(self->fd);
    }
    self->fd = -1;
}

static struct addrinfo *_socket_getadrrinfo(socket_t *self, 
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
        return NULL;
    }

    return results;
}

int socket_bind_and_listen(socket_t *self, const char *service){
    struct addrinfo * results; 
    results = _socket_getadrrinfo(self, 0, service, AI_PASSIVE); // server

    if (!results){
        return -1;
    }

    struct addrinfo * aux = results;
    int fd = -1;
    int bind_error = 0;
    for (; aux && !bind_error; aux = aux->ai_next){
        fd = socket(aux->ai_family, 
                          aux->ai_socktype,
                          aux->ai_protocol);
        if ((bind_error = bind(fd, aux->ai_addr, aux->ai_addrlen)) == 0){
            break;
        }
    }
    
    freeaddrinfo(results);
    if (bind_error || fd < 0) return -1;
    self->fd = fd;

    if (listen(self->fd, 1) == -1){
        return -1;
    }

    return 0;
}

int socket_accept(socket_t *listener, socket_t *peer){
    int peer_fd = accept(listener->fd, 0, 0);
    if (peer_fd < 0){
        return -1;
    }

    peer->fd = peer_fd;
    return 0;
}

int socket_connect(socket_t *self, const char *host, const char *service){
    struct addrinfo * results;
    results = _socket_getadrrinfo(self, 0, service, 0);

    if (!results){
        return -1;
    }

    struct addrinfo *aux = results;
    int connect_error = 0;
    for (; aux; aux = aux->ai_next){
        self->fd = socket(aux->ai_family, 
                          aux->ai_socktype,
                          aux->ai_protocol);
        connect_error = connect(self->fd, aux->ai_addr, aux->ai_addrlen);
        if (!connect_error){
            freeaddrinfo(results);
            return 0;
        }

        if (self->fd != -1){
            close(self->fd);
        }
    }

    if (connect_error){
        freeaddrinfo(results);
        return -1;
    }

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
