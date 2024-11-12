# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Source files
SRC = entropy_generator.c rsa.c implementation.c

# Output executable
OUTPUT = entropy_generator

# Libraries to link
LIBS = -lssl -lcrypto -lgmp

# Default target
all: $(OUTPUT)

# Rule to create the executable
$(OUTPUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUTPUT) $(LIBS)

# Clean rule to remove the generated files
clean:
	rm -f $(OUTPUT)

# Phony targets to prevent conflicts with file names
.PHONY: all clean
