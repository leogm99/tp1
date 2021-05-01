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
    if (!err && (self->serv_sock.fd = socket_accept(&self->listener, 0)) < 0){
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
    size_t parsed_read = 0;
    char *parsed = server_parse(self, received_buffer, size, &parsed_read);
    if (!parsed){
        free(received_buffer);
        return NULL;
    }
    free(received_buffer);

    if (parsed_read == 0){
        *empty_buffer_flag = 1;
        free(parsed);
        return NULL;
    }

    short *mapped = server_map(self, parsed, parsed_read);            
    if (!mapped){
        free(parsed);
        return NULL;
    }
    free(parsed);

    *new_size = parsed_read; 
    short *encoded = cipher_encode(&self->cipher, mapped, 
                                   parsed_read, new_size);
    if (!encoded){
        free(mapped);
        return NULL;
    }
    free(mapped);

    char *final = server_recast(self, encoded, *new_size);
    if (!final){
        free(encoded);
        return NULL;
    }
    free(encoded);
    return final;
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
    char *parsed = parser_parse_string(&self->parser, buffer, size);
    if (!parsed){
        return NULL;
    }        

    *new_size = strlen(parsed);
    return parsed; 
}

short* server_map(server_t *self, char *buffer, size_t size){
    short *mapped_buffer = calloc(size, sizeof(short));
    if (!mapped_buffer){
        return NULL;
    }

    mapper_map(&self->mapper, buffer, mapped_buffer, size);
    return mapped_buffer;
}

char *server_recast(server_t *self, short *buffer, size_t size){
    char *new_buf = calloc(size, sizeof(char));
    if (!new_buf){
        return NULL;
    }

    for (size_t i = 0; i < size; ++i){
        new_buf[i] = (char) buffer[i];
    }
    return new_buf;
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
            return -1;
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
