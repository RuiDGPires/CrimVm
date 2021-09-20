TARGET=crimvm
CC=gcc

C_FILES = $(wildcard src/*.c)
H_FILES = $(wildcard src/*.h)
CFLAGS= 

default: debug

$(TARGET): $(C_FILES) $(H_FILES)
	@$(CC) -Wall $(CFLAGS) -pthread $^ -o $@

debug: CFLAGS += -O0 -D DEBUG -g
debug: $(TARGET)

release: CFLAGS += -O3
release: $(TARGET)

clean:
	rm -f $(TARGET)
