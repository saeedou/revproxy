#include <cutest.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "proxy.h"
#include "proxy.c"


int
tmpfile_open() {
    FILE *file;

    file = tmpfile();
    return fileno(file);
}


void
test_client_to_remote() {
    /* Setup */
    struct connection client;
    fd_set readfds;
    int remote_fd;
    char buff[8] = "01234567";

    client.fd = tmpfile_open();
    client.readflag = 1;
    memset(client.buff, 0, sizeof(client.buff));

    /* read empty file */
    eqint(-1, client_to_remote(&client, remote_fd, &readfds));
    FD_ZERO(&readfds);
    istrue(client.fd > 0);
    FD_SET(client.fd, &readfds);
    remote_fd = tmpfile_open();
    istrue(remote_fd > 0);
    eqint(-1, client_to_remote(&client, remote_fd, &readfds));

    /* previous fd close */
    client.fd = tmpfile_open();

    /* put some data in fd */
    write(client.fd, buff, 8);

    write(client.fd, buff, 8);
    lseek(client.fd, 0, SEEK_SET);

    eqint(1, client.readflag);
    eqint(0, client_to_remote(&client, remote_fd, &readfds));
    eqchr('3', client.buff[3]);
    eqint(0, client.readflag);

    /* teardown */
    close(remote_fd);
}


void
test_remote_to_client() {
    /* setup */
    struct connection client;
    fd_set readfds;
    int remote_fd;
    char buff[8] = "01234567";

    client.fd = tmpfile_open();
    client.readflag = 1;
    memset(client.buff, 0, sizeof(client.buff));
    eqint(-1, remote_to_client(&client, remote_fd, &readfds));

    FD_ZERO(&readfds);
    FD_SET(remote_fd, &readfds);
    eqint(-1, remote_to_client(&client, remote_fd, &readfds));
    
    client.fd = tmpfile_open();
    write(remote_fd, buff, 8);
    lseek(remote_fd, 0, SEEK_SET);
    eqint(0, remote_to_client(&client, remote_fd, &readfds));
    eqchr('4', client.buff[4]);

    /* teardown */
    close(remote_fd);
}


int
main() {
    test_client_to_remote();
    test_remote_to_client();
    return EXIT_SUCCESS;
}
