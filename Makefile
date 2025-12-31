CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g -Iinclude
TARGET = build/main
SRCS = main.c input.c row.c table.c statement.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p build
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build $(OBJS)