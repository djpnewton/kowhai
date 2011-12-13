CC 	   = gcc
CFLAGS = -g -DKOWHAI_DBG

SOCKET_LIB = 
TEST_EXECUTABLE = test
ifeq ($(OS),Windows_NT)
	# on windows we need the winsock library
	SOCKET_LIB = -lws2_32
	# on windows we need the file extension
	TEST_EXECUTABLE = test.exe
endif

all: kowhai test

test: tools/test.o tools/xpsocket.o tools/beep.o
	$(CC) $(LDFLAGS) -o $@ $^ $(SOCKET_LIB) -L. -lkowhai

kowhai: src/kowhai.o src/kowhai_utils.o src/kowhai_protocol.o src/kowhai_protocol_server.o
	ar rs lib$@.a $?

src/kowhai.o: src/kowhai.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/kowhai_utils.o: src/kowhai_utils.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/kowhai_protocol.o: src/kowhai_protocol.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/kowhai_protocol_server.o: src/kowhai_protocol_server.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/test.o: tools/test.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/xpsocket.o: tools/xpsocket.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/beep.o: tools/beep.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f ${TEST_EXECUTABLE} libkowhai.a tools/*.o src/*.o

.PHONY: clean
