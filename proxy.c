// Copyright 2023 Saeed Mahmoodi

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "proxy.h"


int _send_all(int fd, char *buff, int len);


// Send data from client to remote server
int
client_to_remote(struct connection *client) {
    int send_status;
    int readbytes;

    do {
        readbytes = read(client->fd, client->buff, BUFFER_SIZE);
        send_status = _send_all(client->remote_fd, client->buff, readbytes);
        if (send_status == -1) {
            perror("Could not send the data.");
            return -1;
        }
    } while ( readbytes > 0);

    memset(client->buff, 0, BUFFER_SIZE);
    return 0;
}


// Send data from remote server to the client and close the connection
int
remote_to_client(struct connection *client) {
    int send_status;
    int readbytes;

    do {
        readbytes = read(client->remote_fd, client->buff, BUFFER_SIZE);
        send_status = _send_all(client->fd, client->buff, readbytes);
        if (send_status == -1) {
            perror("Could not send the data.");
            return -1;
        }
    } while ( readbytes > 0);

    close(client->fd);
    close(client->remote_fd);
    client->fd = 0;
    client->remote_fd = 0;
    memset(client->buff, 0, BUFFER_SIZE);
    return 0;
}


// send all data from buffer to file descriptor
int
_send_all(int fd, char *buff, int len) {
    int total = 0;
    int bytesleft = len;
    int writebytes;

    while (total < len) {
        writebytes = write(fd, buff + total, bytesleft);
        if (writebytes == -1) {
            break;
        }
        total += writebytes;
        bytesleft -= writebytes;
    }

    return writebytes == -1 ? -1 : 0;
}
