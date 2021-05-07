#include "server.h"

int server_init(server_t *self, const char *service, const char *key){
    char err = 0;
    socket_init(&self->listener);
    mapper_init(&self->mapper);
    cipher_init(&self->cipher);
    parser_init(&self->parser);

    if (cipher_create_key(&self->cipher, &self->mapper, key) < 0){
        err = 1;
    }

    if (!err && (socket_bind_and_listen(&self->listener, service) < 0)){
        err = 1;
    }

    socket_init(&self->serv_sock);
    if (!err && (socket_accept(&self->listener, &self->serv_sock)) < 0){
        err = 1;
    }
    socket_destroy(&self->listener);

    if (err){
        socket_destroy(&self->serv_sock);
        cipher_destroy(&self->cipher);
        mapper_destroy(&self->mapper);
        parser_destroy(&self->parser);
        return err;
    }
    return 0;
}

char* server_prepare_message(server_t *self, char *received_buffer, size_t size,
                             size_t* new_size, char *empty_buffer_flag){
    server_parse(self, received_buffer, size, new_size);
    if (*new_size == 0){
        *empty_buffer_flag = 1;
        free(received_buffer);
        return NULL;
    }

    server_map(self, received_buffer, *new_size);
    char *encoded = cipher_encode(&self->cipher, received_buffer, 
                                   *new_size, new_size);
    if (!encoded){
        free(received_buffer);
        return NULL;
    }
    free(received_buffer);
    return encoded;
}

int server_send(server_t *self, char *buffer, size_t size){
    uint16_t to_send = size * sizeof(char);
    uint16_t client_to_send = ntohs(to_send);
    if (socket_send(&self->serv_sock, &client_to_send, sizeof(uint16_t)) 
                    < sizeof(uint16_t)){
        return -1;
    }

    if (size == 0){
        return 0;
    }

    if (socket_send(&self->serv_sock, buffer, to_send) < to_send){
        return -1;
    }

    return 0;
}

char* server_receive(server_t *self, size_t *size_read){
    uint16_t buf_size = 0;
    if (socket_receive(&self->serv_sock, &buf_size, sizeof(uint16_t)) 
                       < sizeof(uint16_t)){
        return NULL;
    }

    buf_size = ntohs(buf_size);
    char *buffer = calloc(buf_size, sizeof(buf_size));

    if (!buffer){
        return NULL;
    }

    if (socket_receive(&self->serv_sock, buffer, buf_size) < buf_size){
        return NULL;
    }

    *size_read = buf_size; 
    return buffer;
}

char* server_parse(server_t *self, char *buffer, size_t size, size_t *new_size){
    char *parsed = parser_parse_buffer(&self->parser, buffer, size);
    if (!parsed){
        return NULL;
    }        

    *new_size = strlen(parsed);
    return parsed; 
}

char* server_map(server_t *self, char *buffer, size_t size){
    mapper_map(&self->mapper, buffer, buffer, size);
    return buffer;
}

void server_destroy(server_t *self){
    socket_destroy(&self->serv_sock);
    parser_destroy(&self->parser);
    mapper_destroy(&self->mapper);
    cipher_destroy(&self->cipher);
}

int main(int argc, const char *argv[]){
    if (argc != ARGC_SERV) {
        return -1;
    }

    server_t server;
    if (server_init(&server, SERV, KEY) < 0){
        return -1;
    }

    char empty_buffer_flag = 0;
    while (1){
        size_t read = 0;
        char *buffer = server_receive(&server, &read);
        if (!buffer){
            server_destroy(&server);
            break;
        }
        size_t new_size = 0;
        char *message = server_prepare_message(&server, buffer, read, 
                                               &new_size, &empty_buffer_flag);
        if (!message && empty_buffer_flag){
            empty_buffer_flag = 0;
            server_send(&server, 0, 0);
            continue;
        } else if (!message) {
            server_destroy(&server);
            return -1;
        }

        if (server_send(&server, message, new_size) < 0){
            free(message);
            server_destroy(&server);
            return -1;
        }
        free(message);
    }
    server_destroy(&server);
    return 0;
}
