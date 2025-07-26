# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g # -Wall (all warnings), -g (debug symbols)

# Uncomment this if you get linker errors ---> LDFLAGS = -lpthread        # Linker flags

# Executable name
TARGET = main

# Source files
# Automatically find all .c files in the directories
SRCS = $(wildcard http-server/*.c) $(wildcard in-memory-store/*.c)

# Object files (derived from source files)
OBJS = $(SRCS:.c=.o)

# Default target: build the executable
all: $(TARGET)

# Rule to link the object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule to compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to clean up generated files
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean