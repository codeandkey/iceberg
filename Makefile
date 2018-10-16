CC = gcc
CFLAGS = -std=gnu99 -Wall -g -Wno-unused-value -I/usr/include/libxml2
LDFLAGS = -lm -ldl -lSDL2 -lSDL2_image -lopenal -lxml2

SOURCES = $(wildcard src/*.c) $(wildcard src/obj/*.c)
OBJECTS = $(SOURCES:.c=.o)

OUTPUT = iceberg-test

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	@echo ld $(OUTPUT)
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $(OUTPUT)

%.o: %.c
	@echo cc $<
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo clean
	@rm -f $(OUTPUT) $(OBJECTS)
