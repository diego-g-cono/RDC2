// handlers.c

#include "responses.h"
#include "pi.h"
#include "dtp.h"
#include "session.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

void handle_USER(const char *args) {
  ftp_session_t *sess = session_get();

  if (!args || strlen(args) == 0) {
    safe_dprintf(sess->control_sock, MSG_501); // Syntax error in parameters
    return;
  }

  strncpy(sess->current_user, args, sizeof(sess->current_user) - 1);
  sess->current_user[sizeof(sess->current_user) - 1] = '\0';
  safe_dprintf(sess->control_sock, MSG_331); // Username okay, need password
}

void handle_PASS(const char *args) {

  ftp_session_t *sess = session_get();

  if (sess->current_user[0] == '\0') {
    safe_dprintf(sess->control_sock, MSG_503); // Bad sequence of commands
    return;
  }

  if (!args || strlen(args) == 0) {
    safe_dprintf(sess->control_sock, MSG_501); // Syntax error in parameters
    return;
  }

  if (check_credentials(sess->current_user, (char *)args) == 0) {
    sess->logged_in = 1;
    safe_dprintf(sess->control_sock, MSG_230); // User logged in
  } else {
    safe_dprintf(sess->control_sock, MSG_530); // Not logged in
    sess->current_user[0] = '\0'; // Reset user on failed login
    sess->logged_in = 0;
  }
}

void handle_QUIT(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args; // unused

  safe_dprintf(sess->control_sock, MSG_221); // 221 Goodbye.
  sess->current_user[0] = '\0'; // Close session
  close_fd(sess->control_sock, "client socket"); // Close socket
  sess->control_sock = -1;
}

void handle_SYST(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args; // unused

  safe_dprintf(sess->control_sock, MSG_215); // 215 <system type>
}

void handle_TYPE(const char *args) {
  ftp_session_t *sess = session_get();

  if (!args || strlen(args) == 0) {
    safe_dprintf(sess->control_sock, MSG_501); // Syntax error in parameters
    return;
  }

  if (strcmp(args, "I") == 0) {
    sess->transfer_type = TYPE_BINARY;
    safe_dprintf(sess->control_sock, MSG_200);
  } else if (strcmp(args, "A") == 0) {
    sess->transfer_type = TYPE_ASCII;
    safe_dprintf(sess->control_sock, MSG_200);
  } else {
    safe_dprintf(sess->control_sock, MSG_504);
  }

}

void handle_PORT(const char *args) {
  ftp_session_t *sess = session_get();

  if (!args || strlen(args) == 0) {
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  int h1, h2, h3, h4, p1, p2;

  if (sscanf(args, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2) != 6 ||
      h1 < 0 || h1 > 255 || h2 < 0 || h2 > 255 || 
      h3 < 0 || h3 > 255 || h4 < 0 || h4 > 255 ||
      p1 < 0 || p1 > 255 || p2 < 0 || p2 > 255) {
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  char ip_str[INET_ADDRSTRLEN];
  snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", h1, h2, h3, h4);
  int port = p1 * 256 + p2;

  memset(&sess->data_addr, 0, sizeof(sess->data_addr));//para poner los bytes de addr en 0
  sess->data_addr.sin_family = AF_INET;
  sess->data_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, ip_str, &sess->data_addr.sin_addr) <= 0) {//pasa el string de IP a un binario
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  safe_dprintf(sess->control_sock, MSG_200);
}


void handle_RETR(const char *args) {
  ftp_session_t *sess = session_get();

  if (!args || strlen(args) == 0) {
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  // Verificar si el archivo existe
  FILE *file = fopen(args, "rb");
  if (!file) {
    safe_dprintf(sess->control_sock, MSG_550, args);
    return;
  }

  // Obtener el tamaño del archivo
  fseek(file, 0, SEEK_END);
  long filesize = ftell(file);
  rewind(file);

  // Enviar mensaje con tamaño
  safe_dprintf(sess->control_sock, MSG_299, args, filesize);

  // Conectar a la dirección de datos
  int data_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (connect(data_sock, (struct sockaddr*)&sess->data_addr, sizeof(sess->data_addr)) < 0) {
    perror("connect");
    fclose(file);
    return;
  }

  // Enviar el archivo en bloques de 512 bytes
  char buffer[BUFFER_SIZE];
  size_t bytes_read;
  while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
    if (send(data_sock, buffer, bytes_read, 0) != bytes_read) {
      perror("send");
      break;
    }
  }

  fclose(file);
  close(data_sock);

  // Mensaje de finalización
  safe_dprintf(sess->control_sock, MSG_226);
}

void handle_STOR(const char *args) {
  ftp_session_t *sess = session_get();

  if (!args || strlen(args) == 0) {
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  // Abrir archivo para escritura (crear o truncar)
  FILE *file = fopen(args, "wb");
  if (!file) {
    safe_dprintf(sess->control_sock, MSG_550, args);
    return;
  }

  // Mensaje de inicio
  safe_dprintf(sess->control_sock, MSG_150);

  // Conectar al cliente (modo activo)
  int data_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (connect(data_sock, (struct sockaddr*)&sess->data_addr, sizeof(sess->data_addr)) < 0) {
    perror("connect");
    fclose(file);
    return;
  }

  // Recibir datos del cliente
  char buffer[BUFFER_SIZE];
  ssize_t bytes_received;
  while ((bytes_received = recv(data_sock, buffer, sizeof(buffer), 0)) > 0) {
    fwrite(buffer, 1, bytes_received, file);
  }

  fclose(file);
  close(data_sock);

  // Finalizar
  safe_dprintf(sess->control_sock, MSG_226);
}

void handle_NOOP(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args;

  safe_dprintf(sess->control_sock, MSG_200);
}
