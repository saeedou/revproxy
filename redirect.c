// Copyright 2023 Saeed Mahmoodi
#include "redirect.h"


int
redirect_data(int sender_fd, int receiver_fd) {
    int readsize = 0;
    int writesize = 0;

    char buff[BUFFSIZE];

    while (1) {
        readsize = read(sender_fd, buff, BUFFSIZE);
        if (readsize == 0) {
            return 0;
        } else if (readsize < 0) {
            perror("Could not read from clinet.");
            return -1;
        }

        writesize = 0;
        while (readsize != writesize) {
            writesize = write(receiver_fd, buff, readsize - writesize);
            if (writesize < 0) {
                perror("Could not write to server.");
                return -1;
            }
        }
    }
    return -1;
}
