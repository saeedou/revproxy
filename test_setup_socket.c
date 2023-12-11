// Copyright 2023 Saeed Mahmoodi
#include "setup_socket.h"
#include "setup_socket.c"


void
test_create_listen_socket() {
    int listen_socket;
    int connect_socket;
    struct sockaddr_in server_address;
    char ip[] = "127.0.0.1";
    int port = 8090;
    int addr_len = sizeof(server_address);

    istrue(connect(connect_socket, (struct sockaddr *) &server_address,
                addr_len) == -1);

    create_listen_sock(&listen_socket, ip, &port);
    istrue(connect(listen_socket, (struct sockaddr *) &server_address,
                addr_len) == 0);

    close(listen_socket);
}


int
main() {
    test_create_listen_socket();
    return EXIT_SUCCESS;
}
