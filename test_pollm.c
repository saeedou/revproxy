// Copyright 2023 Saeed Mahmoodi
#include <cutest.h>
#include <string.h>
#include <netinet/in.h>

#include "pollm.h"
#include "pollm.c"


void test_set_client() {
    /*
       Assign file descriptor and network address to first empty element in
       array.
    */
    int client_fd = 4;
    int client_addr_size;
    struct client_conn clients[3];
    struct sockaddr_in client_addr;

    client_addr_size = sizeof(struct sockaddr_in);
    memset(&client_addr, 1, client_addr_size);
    for (int i = 0; i < 3; i++) {
        clients[i].pfds.fd = -1;
        memset(&clients[i].client_addr, 0, client_addr_size);
    }

    set_client(clients, client_fd, &client_addr);
    eqint(4, clients[0].pfds.fd);
    istrue(memcmp(&client_addr, &clients[0].client_addr, client_addr_size) \
            == 0);

    eqint(-1, clients[1].pfds.fd);
    isfalse(memcmp(&client_addr, &clients[1].client_addr, client_addr_size) \
            == 0);

    set_client(clients, client_fd, &client_addr);
    eqint(4, clients[1].pfds.fd);
    istrue(memcmp(&client_addr, &clients[1].client_addr, client_addr_size) \
            == 0);
}


void
test_remove_client() {
    int client_fd = 2;
    int client_addr_size;
    struct client_conn clients[2];
    struct sockaddr_in client_addr;

    client_addr_size = sizeof(struct sockaddr_in);
    memset(&client_addr, 1, client_addr_size);

    for (int i = 0; i < 2; i++) {
        clients[i].pfds.fd = i + 1;
        clients[i].client_addr = client_addr;
    }

    eqint(2, clients[1].pfds.fd);
    remove_client(clients, client_fd);
    eqint(-1, clients[1].pfds.fd);

    memset(&client_addr, 1, client_addr_size);
    istrue(memcmp(&clients[1].client_addr, &client_addr, \
                client_addr_size) < 0);
}


int
main() {
    test_set_client();
    test_remove_client();
    return EXIT_SUCCESS;
}
