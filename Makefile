CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Werror
OBJECTS	=	hash.o	aes.o	ll.o	banhammer.o

.PHONY : all
all:	Newspeak	lex.yy.c	lex.yy.o	linux

Newspeak:	$(OBJECTS)

hash.o	:	hash.c
	$(CC) $(CFLAGS) -c	hash.c

aes.o : aes.c
	$(CC) $(CFLAGS) -c	aes.c

ll.o : ll.c
	$(CC) $(CFLAGS) -c	ll.c

banhammer.o : banhammer.c
	$(CC) $(CFLAGS) -c	banhammer.c

lex.yy.c	: words.l
	flex	words.l

lex.yy.o : lex.yy.c
	cc	-c	lex.yy.c

linux	: lex.yy.o banhammer.o
	cc	-g	-o	newspeak	$(OBJECTS)	lex.yy.o	-lfl


.PHONY : clean
clean:
	rm -f newspeak $(OBJECTS) lex.yy.o lex.yy.c
