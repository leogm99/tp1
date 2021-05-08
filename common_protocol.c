#include "common_protocol.h"

int protocol_server_init(protocol_t *self, const char *serv){
    self->cli_sock = NULL;

    socket_t *listener = malloc(sizeof(socket_t));
    if (!listener){
        return -1;
    }

    socket_init(listener);
    if (socket_bind_and_listen(listener, serv) < 0){
        socket_destroy(listener);
        free(listener);
        return -1;
    }

    self->serv_sock = malloc(sizeof(socket_t));
    if (!self->serv_sock){
        socket_destroy(listener);
        free(listener);
        return -1;
    }

    socket_init(self->serv_sock);
    if (socket_accept(listener, self->serv_sock) < 0){
        socket_destroy(listener);
        socket_destroy(self->serv_sock);
        free(listener);
        free(self->serv_sock);
        return -1;
    }

    socket_destroy(listener);
    free(listener);
    return 0;
}

int protocol_client_init(protocol_t *self, const char *host, const char *serv){
    self->serv_sock = NULL;

    self->cli_sock = malloc(sizeof(socket_t));
    if (!self->cli_sock){
        return -1;
    }
    
    socket_init(self->cli_sock);
    if (socket_connect(self->cli_sock, host, serv) < 0){
        socket_destroy(self->cli_sock);
        free(self->cli_sock);
        return -1;
    }
    return 0;
}

int protocol_send(protocol_t *self, char *buffer, 
                      size_t size, Endianess end_func){
    // #polimorfismo
    socket_t *aux = (self->cli_sock) ? self->cli_sock : self->serv_sock;
    uint16_t to_send = end_func(size * sizeof(char));
    if (socket_send(aux, &to_send, sizeof(uint16_t)) 
                    < sizeof(uint16_t)){
        return -1;
    }

    if (socket_send(aux, buffer, size) < size){
        return -1;
    }
    return 0;
}

char* protocol_receive(protocol_t *self, size_t *size_read,
                       Endianess end_func, char *recv_zero_flag){
    socket_t *aux = (self->cli_sock) ? self->cli_sock : self->serv_sock;
    uint16_t buf_size = 0;
    if (socket_receive(aux, &buf_size, sizeof(uint16_t)) < sizeof(uint16_t)){
        return NULL;
    }
    buf_size = end_func(buf_size);

    if (!buf_size && recv_zero_flag){
        *recv_zero_flag = 1;
        return NULL;
    }

    char *buffer = calloc(buf_size, sizeof(char));

    if (!buffer){
        return NULL;
    }

    if (socket_receive(aux, buffer, buf_size) < buf_size){
        return NULL;
    }

    *size_read = buf_size;
    return buffer;
}

int protocol_destroy(protocol_t *self){
    socket_t *aux = (self->cli_sock != NULL) ? self->cli_sock : self->serv_sock;
    if (aux){
        socket_destroy(aux);
        free(aux);
    }
    self->cli_sock = NULL;
    self->serv_sock = NULL;
    return 0;
}
