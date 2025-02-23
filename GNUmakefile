CC := clang
LD := clang

CFLAGS := -Isrc -g -Werror
LDFLAGS := -lm -lelf -lc

SRC_DIR := src
BIN_DIR := bin

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%, $(BIN_DIR)/%, $(SRC:.c=.o))

INSTALL_DIR := /usr/local/bin
LIB_INSTALL_DIR := /usr/local/include/libadi

LIBADI_URL := https://github.com/project-adi/libadi.git

all: $(OBJ)
	$(LD) $(LDFLAGS) -o $(BIN_DIR)/adikit $(OBJ)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

install: all
	mkdir -p $(INSTALL_DIR)
	install -m 755 $(BIN_DIR)/adikit $(INSTALL_DIR)

ifeq ($(wildcard libadi),"")
	git clone $(LIBADI_URL)
endif
	chmod -R a+rw libadi

	mkdir -p $(LIB_INSTALL_DIR)
	cp -r libadi/* $(LIB_INSTALL_DIR)

.PHONY: all install $(BIN_DIR)