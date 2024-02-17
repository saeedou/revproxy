// Copyright 2023 Saeed Mahmoodi
#include <unistd.h>
#include <stdio.h>


int
tmpfile_open() {
    FILE *file;
    file = tmpfile();

    return fileno(file);
}


int _stdio_backup;
#define RECORD_STDIO(stdio, record_stdio_fd, return_val, func, ...) \
    lseek(record_stdio_fd, 0, SEEK_SET); \
    _stdio_backup = dup(fileno(stdio)); \
    dup2(record_stdio_fd, fileno(stdio)); \
    return_val = func(__VA_ARGS__); \
    fflush(stdio); \
    dup2(_stdio_backup, fileno(stdio)); \
    lseek(record_stdio_fd, 0, SEEK_SET);
