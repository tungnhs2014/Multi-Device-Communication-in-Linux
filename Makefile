# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread -Iinc
PIC_FLAG = -fPIC

# Directories
OBJ_DIR = obj
SRC_DIR = src
LIB_DIR = lib
BIN_DIR = bin

# Output executable name
TARGET = $(BIN_DIR)/multi_device

# Automatically detect all .c files
SRC = main.c $(wildcard $(SRC_DIR)/*.c)
OBJ = $(OBJ_DIR)/main.o $(OBJ_DIR)/handler.o

# Libraries
STATIC_LIB = $(LIB_DIR)/static/libhandler.a
SHARED_LIB = $(LIB_DIR)/shared/libhandler.so

# Build the executable using static library
static: $(TARGET)

# Build the executable using shared library
shared: $(OBJ) $(SHARED_LIB)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -L$(LIB_DIR)/shared -lhandler

# Compile .c to .o files with Position Independent Code for shared library
$(OBJ_DIR)/main.o: main.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(PIC_FLAG) -c $< -o $@

$(OBJ_DIR)/handler.o: $(SRC_DIR)/handler.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(PIC_FLAG) -c $< -o $@

# Create static library
$(STATIC_LIB): $(OBJ) | $(LIB_DIR)/static
	ar rcs $(STATIC_LIB) $(OBJ)

# Create shared library
$(SHARED_LIB): $(OBJ) | $(LIB_DIR)/shared
	$(CC) -shared -o $(SHARED_LIB) $(OBJ)

# Link static executable
$(TARGET): $(OBJ) $(STATIC_LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(STATIC_LIB)

# Create directories if they don't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(LIB_DIR)/shared:
	mkdir -p $(LIB_DIR)/shared

$(LIB_DIR)/static:
	mkdir -p $(LIB_DIR)/static

# Clean up
clean:
	rm -f $(OBJ) $(TARGET) $(STATIC_LIB) $(SHARED_LIB)

# Phony targets
.PHONY: static shared clean
