CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g -Iinclude
TARGET = build/main
SRCS = main.c input.c row.c table.c statement.c
OBJS = $(addprefix build/, $(SRCS:.c=.o))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: %.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build