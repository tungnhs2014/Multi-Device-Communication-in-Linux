# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread -Iinc  # -Iinc để chỉ định thư mục chứa header files

# Directories
BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = src
INC_DIR = inc

# Output executable name
TARGET = $(BIN_DIR)/multi_device

# Source files
SRC = main.c $(SRC_DIR)/handler.c

# Object files
OBJ = $(OBJ_DIR)/main.o $(OBJ_DIR)/handler.o

# Default rule to build the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Compile .c files into .o files
$(OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run server
run: $(TARGET)
	./$(TARGET) 8080

# Run client
run_client: $(TARGET)
	./$(TARGET) 9090

# Clean object files and executable
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets
.PHONY: run run_client clean
