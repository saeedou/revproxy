cov:
	@gcc -fprofile-arcs -ftest-coverage test_poll_actions.c \
		-o test_poll_actions
	./test_poll_actions > /dev/null
	gcovr .
	@rm -f *.gc* test_poll_actions

test:
	@make test_poll_actions
	./test_poll_actions
	@rm -f test_poll_actions

lint:
	@prettyc *.c *.h
