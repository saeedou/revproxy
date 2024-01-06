cov:
	@gcc -fprofile-arcs -ftest-coverage test_poll_actions.c \
		-o test_poll_actions
	./test_poll_actions > /dev/null
	# @gcc -fprofile-arcs -ftest-coverage test_redirect.c \
	# 	-o test_redirect
	# ./test_redirect > /dev/null
	gcovr .
	@rm -f *.gc* test_poll_actions test_redirect

test:
	@make test_poll_actions
	# @make test_redirect
	./test_poll_actions
	# ./test_redirect
	@rm -f test_poll_actions test_redirect

lint:
	@prettyc *.c *.h
