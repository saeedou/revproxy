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


int
main() {
    test__send_all();
    return EXIT_SUCCESS;
}
