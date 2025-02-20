CC := clang
LD := clang

CFLAGS := -Isrc -g -Werror
LDFLAGS := -lm -lelf -lc

SRC_DIR := src
BIN_DIR := bin

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%, $(BIN_DIR)/%, $(SRC:.c=.o))

INSTALL_DIR := /usr/local/bin


all: $(OBJ)
	$(LD) $(LDFLAGS) -o $(BIN_DIR)/adikit $(OBJ)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

install: all
	mkdir -p $(INSTALL_DIR)
	install -m 755 $(BIN_DIR)/adikit $(INSTALL_DIR)

.PHONY: all install $(BIN_DIR)
