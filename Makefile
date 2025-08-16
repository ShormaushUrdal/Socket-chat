CC=gcc
CFLAGS=-Wall -pthread
TARGETS=server_discovery client_discovery test_client run_test

all: $(TARGETS)

server_discovery: server.c
	$(CC) $(CFLAGS) -o server_discovery server.c

client_discovery: client.c  
	$(CC) $(CFLAGS) -o client_discovery client.c

test_client: test_client.c
	$(CC) $(CFLAGS) -o test_client test_client.c

run_test: run_test.c
	$(CC) $(CFLAGS) -o run_test run_test.c

clean:
	rm -f $(TARGETS)

test: all
	./run_test 100

test-small: all
	./run_test 10

.PHONY: all clean test test-small
