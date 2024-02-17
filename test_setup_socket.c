// Copyright 2023 Saeed Mahmoodi
#include <cutest.h>
#include <stdio.h>

#include "setup_socket.h"
#include "setup_socket.c"
#include "test_utils.c"


// Flag to control the behavior of __wrap_connect
int use_real_connect = 1;
int mock_connect_value = -1;

int __real_connect(int sockfd, const struct sockaddr *addr, \
        socklen_t addrlen);

int __wrap_connect(int sockfd, const struct sockaddr *addr, \
        socklen_t addrlen) {
    if (use_real_connect) {
        return __real_connect(sockfd, addr, addrlen);
    }

    return mock_connect_value;
}


// Flag to control the behavior of __wrap_socket
int use_real_socket = 1;
int mock_socket_value = -1;

int __real_socket(int domain, int type, int protocol);

int __wrap_socket(int domain, int type, int protocol) {
    if (use_real_socket) {
        return __real_socket(domain, type, protocol);
    }
    return mock_socket_value;
}


// Flag to control the behavior of __wrap_listen
int use_real_listen = 1;
int mock_listen_value = -1;

int __real_listen(int sockfd, int backlog);

int __wrap_listen(int sockfd, int backlog) {
    if (use_real_listen) {
        return __real_listen(sockfd, backlog);
    }

    return mock_listen_value;
}


// Flag to control the behavior of __wrap_bind
int use_real_bind = 1;
int mock_bind_value = -1;

int __real_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

int __wrap_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (use_real_bind) {
        return __real_bind(sockfd, addr, addrlen);
    }

    return mock_bind_value;
}


void
test_create_listen_socket() {
    int listen_socket;
    int connect_socket;
    struct sockaddr_in server_address;
    char ip[] = "127.0.0.1";
    int port = 8091;

    istrue(connect(connect_socket, (struct sockaddr *) &server_address,
                SOCK_ADDR_SIZE) == -1);

    eqint(0, create_listen_sock(&listen_socket, ip, port));
    istrue(connect(listen_socket, (struct sockaddr *) &server_address,
                SOCK_ADDR_SIZE) == 0);

    close(listen_socket);
}


void
test_create_listen_socket_error() {
    int listen_socket;
    char ip[] = "127.0.0.1";
    char record_buff[36];
    int port = 8090;
    int record_stderr = tmpfile_open();
    int return_val = 0;

    /* Error on creating socket */
    use_real_socket = 0;
    RECORD_STDIO(stderr, record_stderr, return_val, create_listen_sock, \
            &listen_socket, ip, port)

    eqint(-1, return_val);
    read(record_stderr, record_buff, 23);
    eqnstr("Socket creation failed.", record_buff, 23);

    /* Teardown */
    use_real_socket = 1;
    return_val = 0;

    /* Error on binding */
    use_real_bind = 0;
    RECORD_STDIO(stderr, record_stderr, return_val, create_listen_sock, \
            &listen_socket, ip, port)

    eqint(-1, return_val);
    read(record_stderr, record_buff, 33);
    eqnstr("Binding socket to address failed.", record_buff, 33);

    /* Teardown */
    use_real_bind = 1;
    return_val = 0;

    /* Error on listening */
    use_real_listen = 0;
    RECORD_STDIO(stderr, record_stderr, return_val, create_listen_sock, \
            &listen_socket, ip, port)

    eqint(-1, return_val);
    read(record_stderr, record_buff, 36);
    eqnstr("Listening for new connection failed.", record_buff, 36);

    /* Teardown */
    use_real_listen = 1;
    return_val = 0;
    close(record_stderr);
}


void
test_create_remote_server_client() {
    // Client part of the proxy to connect to remote server.
    int server_sock;
    int client_sock;
    int server_listen_sock;
    char ip[] = "127.0.0.1";
    int port = 8089;
    struct sockaddr_in client_addr;
    int addr_len;

    eqint(0, create_listen_sock(&server_listen_sock, ip, port));

    eqint(0, create_remote_server_client(&client_sock, ip, port));
    istrue(accept(server_listen_sock, (struct sockaddr *) &client_addr, \
                &addr_len));
}


void
test_create_remote_server_client_error() {
    int client_sock;
    int server_listen_sock;
    char ip[] = "127.0.0.1";
    int port = 8088;
    int record_stderr = tmpfile_open();
    char record_buff[35];
    int return_val = 0;

    create_listen_sock(&server_listen_sock, ip, port);


    /* Socket creation error handling */
    use_real_socket = 0;
    mock_socket_value = -1;
    RECORD_STDIO(stderr, record_stderr, return_val, \
            create_remote_server_client, &client_sock, ip, port)
    eqint(-1, return_val);

    read(record_stderr, record_buff, 23);
    eqnstr("Socket creation failed.", record_buff, 23);

    /* Teardown */
    use_real_socket = 1;

    /* connecting to remote server error handling */
    use_real_connect = 0;
    mock_connect_value = -1;
    RECORD_STDIO(stderr, record_stderr, return_val, \
            create_remote_server_client, &client_sock, ip, port)
    eqint(-1, return_val);

    read(record_stderr, record_buff, 35);
    eqnstr("Could not connect to remote server.", record_buff, 35);

    /* Teardown */
    use_real_connect = 1;
    close(record_stderr);
}


int
main() {
    test_create_listen_socket();
    test_create_listen_socket_error();
    test_create_remote_server_client();
    test_create_remote_server_client_error();
    return EXIT_SUCCESS;
}
