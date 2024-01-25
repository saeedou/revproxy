// Copyright 2023 Saeed Mahmoodi
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cutest.h>

#include "redirect.h"
#include "redirect.c"


#undef BUFFSIZE
#define BUFFSIZE 2


int
tmpfile_open() {
    FILE *file;
    file = tmpfile();

    return fileno(file);
}


// Flag to control the behavior of __wrap_read
int use_real_read = 1;
int mock_read_value = -1;

ssize_t __real_read(int fd, void *buf, size_t count);

ssize_t __wrap_read(int fd, void *buf, size_t count) {
    if (use_real_read) {
        return __real_read(fd, buf, count);
    }
    return mock_read_value;
}


// Flag to control the behavior of __wrap_read
int use_real_write = 1;
int mock_write_value = -1;

ssize_t __real_write(int fd, void *buf, size_t count);

ssize_t __wrap_write(int fd, void *buf, size_t count) {
    if (use_real_write) {
        return __real_write(fd, buf, count);
    }
    return mock_write_value;
}


void
test_redirect_data() {
    /* Arrangement */

    int sender_fd;
    int receiver_fd;
    char receiver_buff[5];

    use_real_read = 1;
    use_real_write = 1;

    receiver_fd = tmpfile_open();

    // A temp file with "1234" content
    sender_fd = tmpfile_open();
    write(sender_fd, "1234", 5);
    lseek(sender_fd, 0, SEEK_SET);

    eqint(0, redirect_data(sender_fd, receiver_fd));
    lseek(receiver_fd, 0, SEEK_SET);
    read(receiver_fd, receiver_buff, 5);
    eqnstr("1234", receiver_buff, 5);

    /* Teardown */
    close(sender_fd);
    close(receiver_fd);
}


void
test_redirect_data_error() {
    /* Arrangement */

    int receiver_fd = tmpfile_open();
    int sender_fd = tmpfile_open();
    int record_stderr = tmpfile_open();
    char record_buff[28];
    int stderr_backup;

    /* Redirecting stderr and takeing log. */
    stderr_backup = dup(fileno(stderr));
    dup2(record_stderr, fileno(stderr));

    /* Error while reading from sender(client) */
    use_real_read = 0;
    eqint(-1, redirect_data(sender_fd, receiver_fd));
    use_real_read = 1;
    fflush(stderr);
    dup2(stderr_backup, fileno(stderr));
    lseek(record_stderr, 0, SEEK_SET);
    read(record_stderr, record_buff, 27);
    eqnstr("Could not read from clinet.", record_buff, 27);

    close(record_stderr);

    /* Redirecting stderr and takeing log. */
    record_stderr = tmpfile_open();
    stderr_backup = dup(fileno(stderr));
    dup2(record_stderr, fileno(stderr));

    /* Error while writing to receiver(server) */
    use_real_write = 0;
    use_real_read = 0;
    mock_read_value = 1;
    eqint(-1, redirect_data(sender_fd, receiver_fd));
    use_real_read = 1;
    fflush(stderr);
    dup2(stderr_backup, fileno(stderr));
    lseek(record_stderr, 0, SEEK_SET);
    read(record_stderr, record_buff, 26);

    eqnstr("Could not write to server.", record_buff, 26);


    /* Teardown */
    use_real_read = 1;
    use_real_write = 1;
    close(record_stderr);
    close(sender_fd);
    close(receiver_fd);
}


int
main() {
    test_redirect_data_error();
    test_redirect_data();
    return EXIT_SUCCESS;
}
