CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic

all: banhammer

banhammer: banhammer.o 
	$(CC) -o banhammer banhammer.o bf.o bv.o ht.o ll.o node.o speck.o parser.o

banhammer.o:
	$(CC) $(CFLAGS) -c banhammer.c bf.c bv.c ht.c ll.c node.c speck.c parser.c

format:
	clang-format -i -style=file *.c *.h

clean:
	rm -f banhammer ./*.o

scan-build: clean
	scan-build make

