// Copyright 2023 Saeed Mahmoodi
#ifndef SETUP_SOCKET_H_
#define SETUP_SOCKET_H_


#ifndef BACKLOG
    #define BACKLOG 256
#endif


void create_listen_sock(int *socket_fd, char *ip, int *port);


#endif  // SETUP_SOCKET_H_
