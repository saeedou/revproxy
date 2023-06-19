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
test_proxy() {
    /* Setup */
    struct connection client1;
    fd_set readfds;
    int fd;
    char buff[8];

    client1.fd = tmpfile_open();
    client1.readflag = 1;
    memset(client1.buff, 0, sizeof(client1.buff));
    memset(buff, 'x', 8);

    /* read empty file */
    eqint(-1, client_to_remote(&client1, fd, &readfds));
    FD_ZERO(&readfds);
    istrue(client1.fd > 0);
    FD_SET(client1.fd, &readfds);
    fd = tmpfile_open();
    istrue(fd > 0);
    eqint(-1, client_to_remote(&client1, fd, &readfds));

    /* previous fd close */
    client1.fd = tmpfile_open();

    /* put some data in fd */
    write(client1.fd, buff, 8);

    write(client1.fd, buff, 8);
    lseek(client1.fd, 0, SEEK_SET);

    eqint(0, client_to_remote(&client1, fd, &readfds));
    eqchr('x', client1.buff[0]);
}

int
main() {
    test_proxy();
    return EXIT_SUCCESS;
}
