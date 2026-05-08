# Project Name
TARGET = simulation 

# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wno-missing-braces -I.
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Source files (all .c files in the directory)
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

# Default Rule
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Compile source files into object files
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Run the project
run: all
	./$(TARGET)