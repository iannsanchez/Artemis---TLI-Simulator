CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
LDFLAGS = -lm
SOURCES = main.c artemis.c fisica.c validaciones.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = artemis

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

%.o: %.c artemis.h
	$(CC) $(CFLAGS) -c $< -o $@

validaciones.o: validaciones.c validaciones.h
	$(CC) $(CFLAGS) -c $< -o $@

test:
	$(CC) $(CFLAGS) -DARTEMIS_TEST fisica.c tests_fisica.c -o test_fisica $(LDFLAGS)
	./test_fisica

clean:
	rm -f $(OBJECTS) $(TARGET) test_fisica
