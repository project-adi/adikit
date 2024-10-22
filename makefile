CC=clang
LD=clang

CFLAGS=-Isrc -g -Werror
LDFLAGS= -lm

SRC=$(wildcard src/*.c)
_OBJ=$(SRC:.c=.o)
OBJ=$(patsubst src/%,bin/%,$(_OBJ))

run: all
	./bin/adikit

all: ${OBJ}
	${LD} ${LDFLAGS} -o bin/adikit ${OBJ}

bin/%.o: src/%.c
	mkdir -p bin
	$(CC) $(CFLAGS) -c -o $@ $< 