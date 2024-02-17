MAKEFLAGS := s

cov:
	@gcc -fprofile-arcs -ftest-coverage test_poll_actions.c \
		-o test_poll_actions
	./test_poll_actions > /dev/null

	@gcc -Wl,--wrap=read -Wl,--wrap=write -fprofile-arcs -ftest-coverage \
		test_redirect.c -o test_redirect
	./test_redirect > /dev/null

	@gcc -fprofile-arcs -ftest-coverage test_validate_ipv4.c \
		-o test_validate_ipv4
	./test_validate_ipv4 > /dev/null

	@gcc -Wl,--wrap=connect -Wl,--wrap=socket -Wl,--wrap=listen \
		-Wl,--wrap=bind -fprofile-arcs -ftest-coverage test_setup_socket.c \
		-o test_setup_socket
	./test_setup_socket > /dev/null

	gcovr .

	@rm -f *.gc* test_poll_actions test_redirect test_validate_ipv4


test:
	@gcc test_poll_actions.c -o test_poll_actions
	./test_poll_actions

	@gcc test_redirect.c -Wl,--wrap=read -Wl,--wrap=write -o test_redirect
	./test_redirect

	@gcc test_validate_ipv4.c -o test_validate_ipv4
	./test_validate_ipv4

	@gcc test_setup_socket.c -Wl,--wrap=connect -Wl,--wrap=socket \
		-Wl,--wrap=listen -Wl,--wrap=bind -o test_setup_socket
	./test_setup_socket

	make clean


lint:
	@prettyc *.c *.h


clean:
	@rm -f *.gc*
	@rm -f test_poll_actions
	@rm -f test_redirect
	@rm -f test_validate_ipv4
	@rm -f test_setup_socket
