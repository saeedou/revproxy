// Copyright 2023 Saeed Mahmoodi
#include <string.h>
#include <stdlib.h>

#include "validate_ipv4.h"


#define TRUE 1
#define FALSE 0
#define DELIMITER "."


int _is_number(char *string) {
    int i = 0;

    while (string[i] != '\0') {
        if (string[i] < 48 || string[i] > 57) {
            return FALSE;
        }
        i++;
    }

    return TRUE;
}


int is_valid_ip(char ip[]) {
    int size = strlen(ip) + 1;
    char ip_arr[size];
    char *token;
    int i = 0;

    strcpy(ip_arr, ip);
    token = strtok(ip_arr, DELIMITER); // NOLINT

    while (token != NULL) {
        if (!_is_number(token) || atoi(token) > 256 || atoi(token) < 1) {
            return 0;
        }

        token = strtok(NULL, DELIMITER); // NOLINT
        i++;
    }

    if (i != 4) {
        return 0;
    }

    return 1;
}
