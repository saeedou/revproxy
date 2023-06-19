compile:
	gcc main.c -o revproxy
test:
	gcc test_proxy.c -o test_proxy
	./test_proxy
	rm test_proxy
