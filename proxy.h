#ifndef PROXY_H
#define PROXY_H


#include "setup_socket.h"


#define BUFFER_SIZE 4096


struct connection {
    int fd;
    int addrlen;
    char buff[BUFFER_SIZE];
    int buffsize;
    int readbytes;
    int writebytes;
    int readflag;
};


int
client_to_remote(struct connection *client, int remote_server_fd,
        fd_set *readfds);


int
remote_to_client(struct connection *client, int remote_server_fd,
        fd_set *readfds);


#endif
