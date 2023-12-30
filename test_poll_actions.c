// Copyright 2023 Saeed Mahmoodi
#include <cutest.h>
#include <string.h>
#include <netinet/in.h>

#define BACKLOG 4

static int is_free_called = 0;
static char *mock_free_ptr;
#define free(ptr) mock_free(ptr)

/* function call and argument checking */
void mock_free(void *ptr) {
    is_free_called = 1;
    mock_free_ptr = ptr;
    // free(ptr);
}


#include "poll_actions.h"
#include "poll_actions.c"


void test_set_client() {
    /*
       Assign file descriptor and network address to first empty element in
       array.
    */
    int client_fd = 4;
    int client_addr_size;
    struct client_conn clients[3];
    struct sockaddr_in client_addr;

    /* arrangement */
    client_addr_size = sizeof(struct sockaddr_in);
    memset(&client_addr, 1, client_addr_size);
    for (int i = 0; i < 2; i++) {
        clients[i].pfds.fd = -1;
        memset(&clients[i].client_addr, 0, client_addr_size);
    }

    /* setting first client */
    eqint(0, set_client(clients, client_fd, &client_addr));
    eqint(4, clients[0].pfds.fd);
    istrue(memcmp(&client_addr, &clients[0].client_addr, client_addr_size) \
            == 0);

    eqint(-1, clients[1].pfds.fd);
    isfalse(memcmp(&client_addr, &clients[1].client_addr, client_addr_size) \
            == 0);

    /* filling the structure */
    eqint(0, set_client(clients, client_fd, &client_addr));
    eqint(4, clients[1].pfds.fd);

    /* Failed because the structure is full */
    eqint(-1, set_client(clients, client_fd, &client_addr));
}


int
test_remove_client() {
    int client_fd = 1;
    int client_addr_size;
    char *hold_ptr;
    struct client_conn clients[2];
    struct sockaddr_in client_addr;

    /* Arrangement */
    client_addr_size = sizeof(struct sockaddr_in);
    memset(&client_addr, 1, client_addr_size);
    clients[1].buff = malloc(1);
    hold_ptr = clients[1].buff;
    for (int i = 0; i < 2; i++) {
        clients[i].pfds.fd = i;
        clients[i].client_addr = client_addr;
    }

    /* Status checking */
    eqint(1, clients[1].pfds.fd);
    isnotnull(clients[1].buff);
    eqint(0, is_free_called);
    isnull(mock_free_ptr);

    /* Reseting client  */
    eqint(0, remove_client(clients, client_fd));
    eqint(-1, clients[1].pfds.fd);
    eqint(0, clients[1].pfds.revents);
    eqint(1, is_free_called);
    istrue(hold_ptr == mock_free_ptr);
    isnull(clients[1].buff);
    istrue(memcmp(&clients[1].client_addr, &client_addr, \
                client_addr_size) < 0);
    eqint(-1, remove_client(clients, 4));

    /* Teardown */
    #undef mock_free
    free(clients[1].buff);
}


void
test_init_pollfds() {
    /* Setup */
    struct pollfd pfds[3];
    struct client_conn clients[3];

    /* Arrangement */
    memset(pfds, 0, sizeof(pfds));
    for (int i = 0; i < 3; i++) {
        clients[i].pfds.fd = i + 2;
    }

    init_pollfds(clients, pfds);
    eqint(2, pfds[0].fd);
}


int
main() {
    test_set_client();
    test_remove_client();
    test_init_pollfds();

    return EXIT_SUCCESS;
}
