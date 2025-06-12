#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "serverpi.h"
#include "serverausftp.h"

int is_valid_command(const char *command) {
    int i = 0;
    while (valid_commands[i] != NULL) {
        if (strcmp(command, valid_commands[i]) == 0) {
            return arg_commands[i];
        }
        i++;
    }
    return -1;
}

/* 
 * Función recv_cmd recibe un comando del cliente y lo separa en operación y parámetro.
 *
 * @param socket_descriptor: Descriptor del socket desde el cual se recibe el comando.
 * @param operation: Cadena donde se almacenará la operación del comando 
 *                   (comandos básicos de ftp, por ejemplo: "USER", "PASS", "LIST", etc.).
 * @param param: Cadena donde se almacenará el parámetro del comando, si existe.
 * @return 0 si se recibió correctamente, 1 si hubo un error.
 */
int recv_cmd(int socket_descriptor, char *operation, char *param) {
    char buffer[BUFSIZE];
    char * token;
    int args_number;

    if (recv(socket_descriptor, buffer, BUFSIZE, 0) < 0) {
        fprintf(stderr, "Error: no se pudo recibir el comando.\n");
        return 1;
    }
    buffer[strcspn(buffer, "\r\n")] = 0;
    token = strtok(buffer, " ");
    if (token == NULL || strlen(token) < 3 || (args_number = is_valid_command(token)) < 0) {
        fprintf(stderr, "Error: comando no válido.\n");
        return 1;
    }
    strcpy(operation, token);
    if (!args_number)
        return 0;
    token = strtok(NULL, " ");
    #if DEBUG 
    printf("par %s\n", token);
    #endif
    if (token != NULL) 
        strcpy(param, token);
    else {
        fprintf(stderr, "Error: se esperaba un argumento para el comando %s.\n", operation);
        return 1;
    }
    return 0;
}