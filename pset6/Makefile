# Compiler and flags
CC = gcc
CFLAGS = -Wall -g
AS = gcc
ASFLAGS = -Wall -g

# Target executable
TARGET = program

# Source files
SRCS = spinlock_test.c spinlock.c tas.S
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compile C source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile assembly files to object files
%.o: %.S
	$(AS) $(ASFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f *.o

# Phony targets
.PHONY: all clean

