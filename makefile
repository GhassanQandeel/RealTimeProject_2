# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -I../include
LDFLAGS = 

# Directories
SRC_DIR = src
BIN_DIR = bin

# Source files
CUSTOMER_SRC = $(SRC_DIR)/customer.c $(SRC_DIR)/config.c
SALLER_SRC = $(SRC_DIR)/saller.c $(SRC_DIR)/config.c
BAKERY_SRC = $(SRC_DIR)/bakery.c $(SRC_DIR)/config.c

# Executables
CUSTOMER_EXE = $(BIN_DIR)/customer
SALLER_EXE = $(BIN_DIR)/saller
BAKERY_EXE = $(BIN_DIR)/bakery

# Config file
CONFIG_FILE = config.txt

# Ensure bin directory exists
$(shell mkdir -p $(BIN_DIR))

.PHONY: all clean run

all: $(CUSTOMER_EXE) $(SALLER_EXE) $(BAKERY_EXE)

$(CUSTOMER_EXE): $(CUSTOMER_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(SALLER_EXE): $(SALLER_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)	

$(BAKERY_EXE): $(BAKERY_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: $(BAKERY_EXE)
	./$(BAKERY_EXE) $(CONFIG_FILE)

clean:
	rm -rf $(BIN_DIR)

debug: CFLAGS += -DDEBUG -O0
debug: all
