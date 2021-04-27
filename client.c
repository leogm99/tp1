#include "client.h"

/*****CLIENT*****/
// ./client <hostname> <servicename> <filename> <- filepath
// echo -e ... | ./client <hostaname> <servicename> - <- stdin

void client_init(client_t *self, socket_t *sock){
    //socket_init(&self->cli_sock);
    self->cli_sock = sock;
}

int client_connect(client_t *self, const char *host, const char *service){
    return socket_connect(self->cli_sock, host, service);
}

int client_read_and_send(client_t *self, file_reader_t *file_reader, char *sent_less_flag){
    int16_t expected = 0;
    char *line = NULL; 

    expected = file_reader_readline(file_reader, &line);

    // si lei 0, ya esta
    // de todas maneras tengo que hacer free
    // segun man 3 getline
    if (!expected) {
        free(line);
        return 0;
    }

    // si lei solo 1
    // significa que lei solamente un \n
    if (expected == 1){
        free(line);
        return 1;
    }

    //uint16_t server_expected = htons(expected);
    if (socket_send(self->cli_sock, &expected, sizeof(uint16_t)) < sizeof(uint16_t)){
        *sent_less_flag = 1;
    }

    if (socket_send(self->cli_sock, line, expected) < expected){
        *sent_less_flag = 1; 
    }

    free(line);
    return expected;
}

int client_receive_and_print(client_t *self){
    uint16_t size = 0;

    if (socket_receive(self->cli_sock, &size, sizeof(uint16_t)) < sizeof(uint16_t)){
        return -1;
    }

    char *buffer = calloc(size, sizeof(char));

    if (!buffer) {
        return -1;
    }

    if (socket_receive(self->cli_sock, buffer, sizeof(char) * size) < sizeof(char) * size){
        free(buffer);
        return -1;
    }

    for (int i = 0; i < size; ++i){
        printf("%c", buffer[i] + 'A');
    }
    puts("");

    free(buffer);
    return 0;
}

void client_destroy(client_t *self){
    self->cli_sock = NULL;
}

int main(int argc, const char *argv[]){
    if (argc != ARGC_CLIENT){
        return -1;
    }

    socket_t cli_sock;
    client_t client;
    file_reader_t file_reader;
    char sent_less_flag = 0;

    socket_init(&cli_sock);
    client_init(&client, &cli_sock);
    file_reader_init(&file_reader, FILE);


    if (client_connect(&client, HOST, SERV) == -1){
        file_reader_destroy(&file_reader);
        client_destroy(&client);
        return -1;
    }

    while (1){
        if (client_read_and_send(&client, &file_reader, &sent_less_flag) <= 0){
            break;
        }

        if (client_receive_and_print(&client) < 0){
            break;
        }
    }

    socket_destroy(&cli_sock);
    file_reader_destroy(&file_reader);
    client_destroy(&client);


    return sent_less_flag;
}
