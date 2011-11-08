CC 	   = gcc
CFLAGS = -g -UKOWHAI_DBG

all: kowhai test

test: src/test.o src/kowhai.o
	$(CC) $(LDFLAGS) -o $@ $^

src/test.o: src/test.c
	$(CC) $(CFLAGS) -c -o $@ $<

kowhai: src/kowhai.o

src/kowhai.o: src/kowhai.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f test.exe src/test.o src/kowhai.o

.PHONY: clean
