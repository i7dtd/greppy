CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
TARGET = greppy
SRC = greppy.c
PREFIX = /usr/local/bin

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

install: $(TARGET)
	mkdir -p $(PREFIX)
	install -m 755 $(TARGET) $(PREFIX)/$(TARGET)
	@echo "$(TARGET) installed successfully to $(PREFIX)"

uninstall:
	rm -f $(PREFIX)/$(TARGET)
	@echo "$(TARGET) removed from $(PREFIX)"

clean:
	rm -f $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild install uninstall