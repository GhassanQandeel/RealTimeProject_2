# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -I../include
LDFLAGS = -lGL -lGLU -lglut -lm

# Directories
SRC_DIR = src
BIN_DIR = bin

# Source files
CUSTOMER_SRC = $(SRC_DIR)/customer.c $(SRC_DIR)/config.c
SALLER_SRC = $(SRC_DIR)/saller.c $(SRC_DIR)/config.c
BAKERY_SRC = $(SRC_DIR)/bakery.c $(SRC_DIR)/config.c
BAKERY_OPENGL_SRC = $(SRC_DIR)/bakery_opengl.c $(SRC_DIR)/config.c
BREAD_BAKE_SRC = $(SRC_DIR)/bread_bake.c $(SRC_DIR)/config.c
CAKE_PRE_SRC = $(SRC_DIR)/cake_pre.c $(SRC_DIR)/config.c
PASTE_PRE_SRC = $(SRC_DIR)/paste_pre.c $(SRC_DIR)/config.c
SANDWICHES_PRE_SRC = $(SRC_DIR)/sandwiches_pre.c $(SRC_DIR)/config.c
SAVORY_PATISS_SRC = $(SRC_DIR)/savory_patiss_pre.c $(SRC_DIR)/config.c
SUPPLIER_SRC = $(SRC_DIR)/supplier.c $(SRC_DIR)/config.c
SWEET_CAKE_BAKE_SRC = $(SRC_DIR)/sweet_cake_bake.c $(SRC_DIR)/config.c
SWEET_PATISS_PRE_SRC = $(SRC_DIR)/sweet_patiss_pre.c $(SRC_DIR)/config.c
SWEET_PRE_SRC = $(SRC_DIR)/sweets_pre.c $(SRC_DIR)/config.c
SWEET_SAVORY_PATISS_BAKE_SRC = $(SRC_DIR)/sweet_savory_patiss_bake.c $(SRC_DIR)/config.c


# Executables
CUSTOMER_EXE = $(BIN_DIR)/customer
SALLER_EXE = $(BIN_DIR)/saller
BAKERY_EXE = $(BIN_DIR)/bakery
BAKERY_OPENGL_EXE = $(BIN_DIR)/bakery_opengl
BREAD_BAKE_EXE = $(BIN_DIR)/bread_bake
CAKE_PRE_EXE = $(BIN_DIR)/cake_pre
PASTE_PRE_EXE = $(BIN_DIR)/paste_pre
SANDWICHES_PRE_EXE = $(BIN_DIR)/sandwiches_pre
SAVORY_PATISS_EXE = $(BIN_DIR)/savory_patiss_pre
SUPPLIER_EXE = $(BIN_DIR)/supplier
SWEET_CAKE_BAKE_EXE = $(BIN_DIR)/sweet_cake_bake
SWEET_PATISS_PRE_EXE = $(BIN_DIR)/sweet_patiss_pre
SWEET_PRE_EXE = $(BIN_DIR)/sweets_pre
SWEET_SAVORY_PATISS_BAKE_EXE = $(BIN_DIR)/sweet_savory_patiss_bake

# Config file
CONFIG_FILE = config.txt

# Ensure bin directory exists
$(shell mkdir -p $(BIN_DIR))

.PHONY: all clean run

all: $(CUSTOMER_EXE) $(SALLER_EXE) $(BAKERY_OPENGL_EXE) $(BREAD_BAKE_EXE) $(CAKE_PRE_EXE) $(PASTE_PRE_EXE) $(SANDWICHES_PRE_EXE) $(SAVORY_PATISS_EXE) $(SUPPLIER_EXE) $(SWEET_CAKE_BAKE_EXE) $(SWEET_PATISS_PRE_EXE) $(SWEET_PRE_EXE) $(SWEET_SAVORY_PATISS_BAKE_EXE)

$(CUSTOMER_EXE): $(CUSTOMER_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(SALLER_EXE): $(SALLER_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)	

$(BAKERY_EXE): $(BAKERY_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BAKERY_OPENGL_EXE): $(BAKERY_OPENGL_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BREAD_BAKE_EXE): $(BREAD_BAKE_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(CAKE_PRE_EXE): $(CAKE_PRE_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(PASTE_PRE_EXE): $(PASTE_PRE_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(SANDWICHES_PRE_EXE): $(SANDWICHES_PRE_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(SAVORY_PATISS_EXE): $(SAVORY_PATISS_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(SUPPLIER_EXE): $(SUPPLIER_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(SWEET_CAKE_BAKE_EXE): $(SWEET_CAKE_BAKE_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(SWEET_PATISS_PRE_EXE): $(SWEET_PATISS_PRE_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(SWEET_PRE_EXE): $(SWEET_PRE_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)	

$(SWEET_SAVORY_PATISS_BAKE_EXE): $(SWEET_SAVORY_PATISS_BAKE_SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: $(BAKERY_EXE)
	./$(BAKERY_EXE) $(CONFIG_FILE)

clean:
	rm -rf $(BIN_DIR)

debug: CFLAGS += -DDEBUG -O0
debug: all
