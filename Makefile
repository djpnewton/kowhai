CC 	   = gcc
CFLAGS = -g -UKOWHAI_DBG

SOCKET_LIB = 
ifeq ($(OS),Windows_NT)
	# on windows we need the winsock library
	SOCKET_LIB = -lws2_32
endif

all: kowhai test

test: tools/test.o src/kowhai.o src/kowhai_protocol.o tools/xpsocket.o
	$(CC) $(LDFLAGS) -o $@ $^ $(SOCKET_LIB)

kowhai: src/kowhai.o src/kowhai_protocol.o

src/kowhai.o: src/kowhai.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/kowhai_protocol.o: src/kowhai_protocol.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/test.o: tools/test.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/xpsocket.o: tools/xpsocket.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f test.exe tools/test.o tools/xpsocket.o src/kowhai.o src/kowhai_protocol.o

.PHONY: clean
