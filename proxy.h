#ifndef PROXY_H
#define PROXY_H


#include "setup_socket.h"


#define BUFFER_SIZE 64


struct connection {
    int fd;
    int addrlen;
    char buff[BUFFER_SIZE];
    int buffsize;
    int readbytes;
    int writebytes;
};


void
proxy(struct connection *clients, int remote_server_fd, int max_client_num,
        fd_set *readfds, fd_set *writefds);



#endif
