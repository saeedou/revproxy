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
    int fd;
    char buff[8];

    client.fd = tmpfile_open();
    client.readflag = 1;
    memset(client.buff, 0, sizeof(client.buff));
    memset(buff, 'x', 8);

    /* read empty file */
    eqint(-1, client_to_remote(&client, fd, &readfds));
    FD_ZERO(&readfds);
    istrue(client.fd > 0);
    FD_SET(client.fd, &readfds);
    fd = tmpfile_open();
    istrue(fd > 0);
    eqint(-1, client_to_remote(&client, fd, &readfds));

    /* previous fd close */
    client.fd = tmpfile_open();

    /* put some data in fd */
    write(client.fd, buff, 8);

    write(client.fd, buff, 8);
    lseek(client.fd, 0, SEEK_SET);

    eqint(1, client.readflag);
    eqint(0, client_to_remote(&client, fd, &readfds));
    eqchr('x', client.buff[0]);
    eqint(0, client.readflag);
}


void
test_remote_to_client() {
    /* setup */
}


int
main() {
    test_client_to_remote();
    return EXIT_SUCCESS;
}
