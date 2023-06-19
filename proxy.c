#include "proxy.h"


int
client_to_remote(struct connection *client, int remote_server_fd,
        fd_set *readfds) {
    if (FD_ISSET(client->fd, readfds)) {
        if ((client->readbytes = read(client->fd,
                        client->buff, BUFFER_SIZE)) == 0) {
            close(client->fd);
            client->fd = 0;
            memset(client->buff, 0, BUFFER_SIZE);
            return -1;

        } else {
            write(remote_server_fd, client->buff,
                    client->readbytes);
            client->readflag = 0;
            return 0;
        }
    }
    return -1;
}


int
remote_to_client(struct connection *client, int remote_server_fd,
        fd_set *readfds) {
    if (FD_ISSET(remote_server_fd, readfds)) {
        if ((client->readbytes = read(remote_server_fd,
                        client->buff, BUFFER_SIZE)) == 0) {
            close(client->fd);
            client->fd = 0;
            memset(client->buff, 0, BUFFER_SIZE);
            return -1;

        } else {
            write(client->fd, client->buff,
                    client->readbytes);
            client->readflag = 1;
            return 0;
        }
    }
    return -1;
}
