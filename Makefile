CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
TARGET = greppy
OBJS = greppy.c

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

clean:
	rm -rf $(TARGET)

rebuild: clean all

.PHONY: all rebuild clean