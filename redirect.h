// Copyright 2023 Saeed Mahmoodi
#ifndef REDIRECT_H_
#define REDIRECT_H_

#include "poll_actions.h"

#ifndef HANDLE_ERROR
#define HANDLE_ERROR(msg) \
           do {perror(msg); exit(EXIT_FAILURE);} while (0)
#endif


int redirect_data(int sender_fd, int receiver_fd);


#endif  // REDIRECT_H_
