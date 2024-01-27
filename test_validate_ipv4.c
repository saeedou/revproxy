// Copyright 2023 Saeed Mahmoodi
#include <cutest.h>

#include "validate_ipv4.h"
#include "validate_ipv4.c"


void
test_is_valid_ip() {
    istrue(is_valid_ip("1.2.3.4"));
    istrue(is_valid_ip("111.222.233.144"));

    isfalse(is_valid_ip(" 111.222.233.244 "));
    isfalse(is_valid_ip("a.b.c.d"));
    isfalse(is_valid_ip("1.1.1"));
}


int
main() {
    test_is_valid_ip();
    return EXIT_SUCCESS;
}
