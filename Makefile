CC 	   ?= gcc
AR 	   ?= ar
CFLAGS += -g -DKOWHAI_DBG -fPIC
## ARM stuff
#CC 	   = arm-none-eabi-gcc
#CFLAGS    = -fpic -static

KOWHAI_VER = 0.0.1

LIBS = 
TEST_EXECUTABLE = test
ifeq ($(OS),Windows_NT)
	# on windows we need the winsock library
	LIBS += -lws2_32
	# on windows we need the multimedia library
	LIBS += -lwinmm
	# on windows we need the file extension
	TEST_EXECUTABLE = test.exe
else
	# on linux we need pthreads
	LIBS += -lpthread
endif

all: jsmn libkowhai.a test

test: tools/test.o tools/xpsocket.o tools/beep.o tools/timer.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) -L. -Wl,-Bstatic -lkowhai -Wl,-Bdynamic

libkowhai.a: src/kowhai.o src/kowhai_log.o src/kowhai_protocol.o src/kowhai_protocol_server.o src/kowhai_serialize.o src/kowhai_utils.o 3rdparty/jsmn/jsmn.o
	$(AR) rs $@ $?

libkowhai.so: src/kowhai.c src/kowhai_log.c src/kowhai_protocol.c src/kowhai_protocol_server.c src/kowhai_serialize.c src/kowhai_utils.c 3rdparty/jsmn/jsmn.c
	# make a shared library for linux/mac (@todo versioning)
	$(CC) $(CFLAGS) -shared -Wl,-soname,$@ -o $@ $?

jsmn: 3rdparty/jsmn/jsmn.o
	$(AR) rs lib$@.a $?

3rdparty/jsmn/jsmn.o: 3rdparty/jsmn/jsmn.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/kowhai.o: src/kowhai.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/kowhai_log.o: src/kowhai_log.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/kowhai_protocol.o: src/kowhai_protocol.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/kowhai_protocol_server.o: src/kowhai_protocol_server.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/kowhai_serialize.o: src/kowhai_serialize.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/kowhai_utils.o: src/kowhai_utils.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/test.o: tools/test.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/xpsocket.o: tools/xpsocket.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/beep.o: tools/beep.c
	$(CC) $(CFLAGS) -c -o $@ $<

src/timer.o: tools/timer.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f ${TEST_EXECUTABLE} libjsmn.a libkowhai.a libkowhai.so tools/*.o src/*.o 3rdparty/jsmn/*.o

.PHONY: clean
