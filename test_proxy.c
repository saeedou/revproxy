#include <cutest.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>


static int write_mock = 0;
#define write(...) 0; \
    switch (write_mock) { \
        case 0: \
            writebytes = write(__VA_ARGS__); \
            break; \
        case 1: \
            writebytes = -1; \
        break; \
    }


#include "proxy.h"
#include "proxy.c"
#undef write


// Using smaller buffer to make testing easier.
#undef BUFFER_SIZE
#define BUFFER_SIZE 8


int
tmpfile_open() {
    FILE *file;

    // Create stream descriptor and return file descriptor
    file = tmpfile();
    return fileno(file);
}


void
test__send_all() {
    // Setup
    int fd = tmpfile_open();
    char buffer[] = "foo";
    char read_buffer[3];
    int len = 3;

    // Use the original write()
    write_mock = 0;
    eqint(0, _send_all(fd, buffer, len));

    lseek(fd, 0, SEEK_SET);
    read(fd, read_buffer, len);
    eqnstr("foo", read_buffer, 3);

    write_mock = 1;
    eqint(-1, _send_all(fd, buffer, len));

    // Teardown
    close(fd);
    write_mock = 0;
}


void
test_client_to_remote() {
    /* Setup */
    struct connection client;
    char buff[8];
    int zero_arr[sizeof(client.buff)];
    memset(zero_arr, 0, sizeof(zero_arr));
    client.fd = tmpfile_open();
    client.remote_fd = tmpfile_open();

    write(client.fd, "12345678", 8);
    lseek(client.fd, 0, SEEK_SET);

    // send the data to remote
    eqint(0, client_to_remote(&client));
    lseek(client.remote_fd, 0, SEEK_SET);
    read(client.remote_fd, buff, 8);
    eqstr("12345678", buff);
    eqbin(zero_arr, client.buff, sizeof(client.buff));

    // send the data to client
    write(client.fd, "12345678", 8);
    lseek(client.fd, 0, SEEK_SET);

    write_mock = 1;
    eqint(-1, client_to_remote(&client));

    /* teardown */
    close(client.fd);
    close(client.remote_fd);
    write_mock = 0;
}


void
test_remote_to_client() {
    /* setup */
    struct connection client;
    client.fd = tmpfile_open();
    client.remote_fd = tmpfile_open();
    char buff_foo[8] = "87654321";
    write(client.remote_fd, buff_foo, 8);
    lseek(client.remote_fd, 0, SEEK_SET);

    // assert
    eqint(0, remote_to_client(&client));

    /* setup */
    char buff_bar[8];
    lseek(client.fd, 0, SEEK_SET);
    read(client.fd, buff_bar, 8);

    /* assert */
    eqnstr(buff_foo, buff_bar, 8);

    /* setup */
    write_mock = 1;
    lseek(client.remote_fd, 0, SEEK_SET);
    
    /* assert */
    eqint(-1, remote_to_client(&client));

    /* teardown */
    close(client.remote_fd);
    close(client.fd);
    write_mock = 0;
}


int
main() {
    test__send_all();
    test_client_to_remote();
    test_remote_to_client();
    return EXIT_SUCCESS;
}
