// Copyright 2023 Saeed Mahmoodi

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <fcntl.h>

#include "setup_socket.h"
#include "setup_socket.c"
#include "proxy.h"
#include "proxy.c"


int
main() {
    int main_socket_fd;
    int max_client_num = 64;
    struct connection clients[max_client_num];
    struct connection client_conn;
    struct sockaddr_in client_addr;
    int max_fd;
    int fd;
    int i;
    int event;
    int readbytes;
    int sock_flags;
    int remote_fd;
    fd_set readfds;

    int remote_port = 8080;
    char remote_ip[] = "127.0.0.1";

    for (i = 0; i < max_client_num; i++) {
        clients[i].fd = 0;
        clients[i].buffsize = BUFFER_SIZE;
        memset(clients[i].buff, 0, BUFFER_SIZE);
    }
    create_listen_sock(&main_socket_fd);

    while (1) {
        // Adding file descriptors to readfds
        FD_ZERO(&readfds);
        FD_SET(main_socket_fd, &readfds);
        max_fd = main_socket_fd;

        for (i = 0; i < max_client_num; i++) {
            fd = clients[i].fd;
            remote_fd = clients[i].remote_fd;

            if (fd > 0) {
                FD_SET(fd, &readfds);
                FD_SET(remote_fd, &readfds);
            }

            if (fd > max_fd) {
                max_fd = remote_fd;
            }
        }

        // Waiting for events
        event = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (event == -1) {
            handle_error("Event loop failed.");
        }

        if (FD_ISSET(main_socket_fd, &readfds)) {
            // Accepting new connection
            client_conn.addrlen = sizeof(client_addr);
            client_conn.fd = accept(main_socket_fd,
                    (struct sockaddr *) &client_addr, &client_conn.addrlen);
            client_conn.remote_fd = create_remote_sock(remote_ip,
                    remote_port);

            sock_flags = fcntl(client_conn.fd, F_GETFL, 0);
            fcntl(client_conn.fd, F_SETFL, sock_flags | O_NONBLOCK);

            sock_flags = fcntl(client_conn.remote_fd, F_GETFL, 0);
            fcntl(client_conn.remote_fd, F_SETFL, sock_flags | O_NONBLOCK);

            if (client_conn.fd == -1) {
                handle_error("Accepting connection failed.");
            }

            for (i = 0; i < max_client_num; i++) {
                if (clients[i].fd == 0) {
                    clients[i] = client_conn;
                    break;
                }
            }
        } else {
            // Handling data exchange
            for (i = 0; i < max_client_num; i++) {
                if (FD_ISSET(clients[i].remote_fd, &readfds)) {
                    remote_to_client(clients + i);
                }

                if (FD_ISSET(clients[i].fd, &readfds)) {
                    client_to_remote(clients + i);
                }
            }
        }
    }

    close(main_socket_fd);
    return EXIT_SUCCESS;
}
