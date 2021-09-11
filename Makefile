TARGET=a.out
CC=gcc

C_FILES = $(wildcard src/*.c)

CFLAGS= 

default: debug

$(TARGET): $(C_FILES)
	@$(CC) $(CFLAGS) $^ -o $@

debug: CFLAGS += -O0 -D DEBUG -g
debug: $(TARGET)

clean:
	rm -f $(TARGET)
