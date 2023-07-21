// Copyright 2023 Saeed Mahmoodi

#ifndef SETUP_SOCKET_H
#define SETUP_SOCKET_H


#define PORT 8011
#define BACKLOG 64
#define IP "127.0.0.1"
#define handle_error(msg) \
           do {perror(msg); exit(EXIT_FAILURE);} while (0)

void
create_listen_sock(int *main_socket_fd);


int
create_remote_sock(char *ip, int port);


#endif
