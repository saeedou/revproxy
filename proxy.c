#include "proxy.h"


void
proxy(struct connection *clients, int remote_server_fd, int max_client_num,
        fd_set *readfds, fd_set *writefds) {
    int i;
    int readbytes;

    for (i = 0; i < max_client_num; i++) {
        if (FD_ISSET(clients[i].fd, readfds)) {
            if ((clients[i].readbytes = read(clients[i].fd,
                            clients[i].buff, BUFFER_SIZE)) == 0) {
                close(clients[i].fd);
                clients[i].fd = 0;
                memset(clients[i].buff, 0, BUFFER_SIZE);

            } else {
                write(remote_server_fd, clients[i].buff, clients[i].readbytes);
                readbytes = read(remote_server_fd, clients[i].buff,
                        clients[i].buffsize);
                write(clients[i].fd, clients[i].buff, readbytes);

                close(clients[i].fd);
                clients[i].fd = 0;
                memset(clients[i].buff, 0, BUFFER_SIZE);
            }
        }
    }
}
