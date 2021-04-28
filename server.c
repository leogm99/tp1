#include "server.h"

// ./server <servicename> <key>

void server_init(server_t *self){
    socket_init(&self->listener);
    socket_init(&self->serv_sock);

    parser_init(&self->parser);
    mapper_init(&self->mapper);

    cipher_init(&self->cipher);
}

void server_create_key(server_t *self, const char *raw_key){
    cipher_create_key(&self->cipher, &self->mapper, raw_key); 
}

int server_bind_and_listen(server_t *self, const char *service){
    return socket_bind_and_listen(&self->listener, service);
}

int server_accept(server_t *self){
    if ((self->serv_sock.fd = socket_accept(&self->listener, 0)) < 0){
        return -1;
    }
    return 0;
}

int server_send(server_t *self, char *buffer, size_t size){
    uint16_t to_send = size * sizeof(char);

    if (socket_send(&self->serv_sock, &to_send, sizeof(uint16_t)) 
                    < sizeof(uint16_t)){
        return -1;
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

    map(&self->mapper, buffer, mapped_buffer, size);
    return mapped_buffer;
}

short* server_encode(server_t *self, short *buffer, 
                     const size_t size, size_t *new_size){
    short *encoded = cipher_encode(&self->cipher, buffer, size, new_size);
    return encoded;
}

char *server_recast(server_t *self, short *buffer, size_t size){
    char *new_buf = calloc(size, sizeof(char));
    //printf("SERVER_RECAST SIZE: %ld", size);
    if (!new_buf){
        return NULL;
    }

    for (size_t i = 0; i < size; ++i){
        new_buf[i] = (char) buffer[i];
    }
    return new_buf;
}

void server_destroy(server_t *self){
    socket_destroy(&self->listener);
    socket_destroy(&self->serv_sock);
 
    parser_destroy(&self->parser);
    mapper_destroy(&self->mapper);

    cipher_destroy(&self->cipher);
}

int main(int argc, const char *argv[]){
    server_t server;
    server_init(&server);

    server_create_key(&server, KEY);
    if (server_bind_and_listen(&server, SERV) < 0){
        server_destroy(&server);
        return -1;
    }

    if (server_accept(&server) < 0){
        server_destroy(&server);
        return -1;
    }

    while (1){
        size_t read = 0;
        size_t parsed_read = 0;

        char *buffer = server_receive(&server, &read);
        if (!buffer){
            break;
        }

        char *parsed = server_parse(&server, buffer, read, &parsed_read);
        if (!parsed){
            free(buffer);
            break;
        }
        free(buffer);

        short *mapped = server_map(&server, parsed, parsed_read);            
        if (!mapped){
            free(parsed);
            break;
        }

        free(parsed);        

        size_t new_size = parsed_read; 
        short *encoded = server_encode(&server, mapped, parsed_read, &new_size);

        if (!encoded){
            free(mapped);
            break;
        }
        free(mapped);


        char *final = server_recast(&server, encoded, new_size);

        if (!final){
            free(encoded);
            break;
        }

        free(encoded);

        server_send(&server, final, new_size);
        free(final);
    }
    server_destroy(&server);
}
