#include "client.h"

/*****CLIENT*****/
// ./client <hostname> <servicename> <filename> <- filepath
// echo -e ... | ./client <hostaname> <servicename> - <- stdin

int client_init(client_t *self, const char *filename, 
                const char *hostname, const char *service){
    socket_init(&self->cli_sock);
    if (file_reader_init(&self->file_reader, filename) < 0){
        return -1;
    }

    if (socket_connect(&self->cli_sock, hostname, service) == -1){
        file_reader_destroy(&self->file_reader);
        socket_destroy(&self->cli_sock);
        return -1;
    }

    mapper_init(&self->mapper);
    mapper_invert(&self->mapper);
    return 0;
}

int client_read_and_send(client_t *self, char *sent_less_flag, 
                         char *newline_flag){
    int16_t expected = 0;
    char *line = NULL; 

    expected = file_reader_readline(&self->file_reader, &line);

    // si lei 0, ya esta
    // de todas maneras tengo que hacer free
    // segun man 3 getline
    if (!expected) {
        free(line);
        return 0;
    }

    uint16_t server_expected = htons(expected);
    if (socket_send(&self->cli_sock, &server_expected, sizeof(uint16_t)) 
                    < sizeof(uint16_t)){
        *sent_less_flag = 1;
    }

    if (socket_send(&self->cli_sock, line, expected) < expected){
        *sent_less_flag = 1; 
    }

    free(line);
    return expected;
}

int client_receive_and_print(client_t *self){
    char newline = '\n';
    uint16_t size = 0;

    if (socket_receive(&self->cli_sock, &size, sizeof(uint16_t)) 
                       < sizeof(uint16_t)){
        return -1;
    }

    size = htons(size);

    if (size == 0){
        fwrite(&newline, 1, 1, stdout);
        return 0;
    }

    char *buffer = calloc(size, sizeof(char));
    if (!buffer) {
        return -1;
    }

    if (socket_receive(&self->cli_sock, buffer, sizeof(char) * size) 
                       < sizeof(char) * size){
        free(buffer);
        return -1;
    }

    char *mapped_buffer = calloc(size, sizeof(char));

    if (!mapped_buffer){
        free(buffer);
        return -1;
    }

    mapper_map(&self->mapper, buffer, mapped_buffer, size);

    fwrite(mapped_buffer, sizeof(char), size, stdout);
    fwrite(&newline, sizeof(char), 1, stdout);

    free(buffer);
    free(mapped_buffer);
    return 0;
}

void client_destroy(client_t *self){
    socket_destroy(&self->cli_sock);
    file_reader_destroy(&self->file_reader);
    mapper_destroy(&self->mapper);
}

int main(int argc, const char *argv[]){
    if (argc != ARGC_CLIENT){
        return -1;
    }

    client_t client;
    if (client_init(&client, FILE, HOST, SERV) < 0){
        return -1;
    }

    char sent_less_flag = 0;
    char newline_flag = 0;
    while (1){
        if ((client_read_and_send(&client, &sent_less_flag, 
                                  &newline_flag)) <= 0){
            break;
        }

        if (newline_flag){
            newline_flag = 0;
            continue;
        }

        if (client_receive_and_print(&client) < 0){
            break;
        }
    }

    client_destroy(&client);
    return sent_less_flag;
}
