CC = gcc
CFLAGS = -Wall -pthread
TARGET:= multi_device

# File source
SRC:= multi_device.c

# Build
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Run server
run: $(TARGET)
	./$(TARGET) 8080

# Run client
run_client: $(TARGET)
	./$(TARGET) 9090

# clean oject
clean:
	rm -f $(TARGET)

.PHONY:= run run_client clean
