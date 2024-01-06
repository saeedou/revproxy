// Copyright 2023 Saeed Mahmoodi
#include <cutest.h>
#include <string.h>
#include <netinet/in.h>

#define BACKLOG 4


static int free_call_count = 0;
static void *mock_free_ptr[2];
#define free(ptr) mock_free(ptr)

/* function call and argument checking */
void mock_free(void *ptr) {
    free_call_count++;
    mock_free_ptr[free_call_count - 1] = ptr;
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


void
test_remove_client() {
    int client_fd = 1;
    int client_addr_size;
    char *hold_ptr1;
    char *hold_ptr2;
    struct client_conn clients[2];
    struct sockaddr_in client_addr;

    /* Arrangement */
    client_addr_size = sizeof(struct sockaddr_in);
    memset(&client_addr, 1, client_addr_size);
    clients[1].readbuff = malloc(1);
    clients[1].writebuff = malloc(1);
    hold_ptr1 = clients[1].readbuff;
    hold_ptr2 = clients[1].writebuff;
    for (int i = 0; i < 2; i++) {
        clients[i].pfds.fd = i;
        clients[i].client_addr = client_addr;
    }

    /* Status checking */
    eqint(1, clients[1].pfds.fd);
    eqint(0, free_call_count);
    isnull(mock_free_ptr[1]);
    isnull(mock_free_ptr[0]);

    /* Assert resetting client  */
    eqint(0, remove_client(clients, client_fd));
    eqint(-1, clients[1].pfds.fd);
    eqint(0, clients[1].pfds.revents);
    eqint(2, free_call_count);
    istrue(hold_ptr1 == mock_free_ptr[0]);
    istrue(hold_ptr2 == mock_free_ptr[1]);
    isnull(clients[1].readbuff);
    isnull(clients[1].writebuff);
    istrue(memcmp(&clients[1].client_addr, &client_addr, \
                client_addr_size) < 0);
    eqint(-1, remove_client(clients, 4));

    /* Teardown */
    #undef free
    free(clients[1].readbuff);
    free(clients[1].writebuff);
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


void
test_which_client() {
    struct client_conn clients[2];

    clients[0].pfds.fd = 0;
    clients[1].pfds.fd = 1;


    eqint(1, which_client(clients, 1));
    eqint(-1, which_client(clients, 2));
}


int
main() {
    test_set_client();
    test_remove_client();
    test_init_pollfds();
    test_which_client();

    return EXIT_SUCCESS;
}
