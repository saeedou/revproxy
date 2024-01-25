MAKEFLAGS := s

cov:
	@gcc -fprofile-arcs -ftest-coverage test_poll_actions.c \
		-o test_poll_actions
	./test_poll_actions > /dev/null
	@gcc -Wl,--wrap=read -Wl,--wrap=write -fprofile-arcs -ftest-coverage \
		test_redirect.c -o test_redirect
	./test_redirect > /dev/null
	gcovr .
	@rm -f *.gc* test_poll_actions test_redirect


test:
	@gcc test_poll_actions.c -o test_poll_actions
	./test_poll_actions
	@gcc test_redirect.c -Wl,--wrap=read -Wl,--wrap=write -o test_redirect
	./test_redirect
	make clean


lint:
	@prettyc *.c *.h


clean:
	@rm -f test_poll_actions
	@rm -f test_redirect
