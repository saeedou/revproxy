// Copyright 2023 Saeed Mahmoodi

#ifndef PROXY_H
#define PROXY_H


#include "setup_socket.h"


#define BUFFER_SIZE 4096


struct connection {
    int fd;
    int remote_fd;
    int addrlen;
    char buff[BUFFER_SIZE];
    int buffsize;
};


int
client_to_remote(struct connection *client);


int
remote_to_client(struct connection *client);


#endif
