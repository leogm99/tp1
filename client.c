#include "client.h"

/*****CLIENT*****/
// ./client <hostname> <servicename> <filename> <- filepath
// echo -e ... | ./client <hostaname> <servicename> - <- stdin

int client_init(client_t *self, const char *filename, 
                const char *hostname, const char *service){
    if (file_reader_init(&self->file_reader, filename) < 0){
        return -1;
    }

    if (protocol_client_init(&self->protocol, hostname, service) < 0){
        file_reader_destroy(&self->file_reader);
        return -1;
    }

    mapper_init(&self->mapper);
    mapper_invert(&self->mapper);
    return 0;
}

int client_read_and_send(client_t *self, char *sent_less_flag, 
                         char *newline_flag){
    size_t expected = 0;
    char *line = NULL; 

    expected = file_reader_readline(&self->file_reader, &line);
    if (errno == ENOMEM || errno == EINVAL){
        return -1;
    }
    // si lei 0, ya esta
    // de todas maneras tengo que hacer free
    // segun man 3 getline
    if (!expected) {
        free(line);
        return 0;
    }
    if (protocol_send(&self->protocol, line, expected, htons) < 0){
        return -1;
    }

    free(line);
    return expected;
}

int client_receive_and_print(client_t *self){
    char newline = '\n';

    size_t size = 0;
    char zero_flag = 0;
    char *buffer = protocol_receive(&self->protocol, &size, htons, &zero_flag);

    if (!buffer && zero_flag){
        fwrite(&newline, 1, 1, stdout);
        return 0;
    } else if (!buffer){
        return -1;
    }

    mapper_map(&self->mapper, buffer, buffer, size);
    fwrite(buffer, sizeof(char), size, stdout);
    fwrite(&newline, sizeof(char), 1, stdout);

    free(buffer);
    return 0;
}

void client_destroy(client_t *self){
    protocol_destroy(&self->protocol);
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
        if (client_receive_and_print(&client) < 0){
            break;
        }
    }

    client_destroy(&client);
    return sent_less_flag || errno;
}
