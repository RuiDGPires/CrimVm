TARGET=crimvm
CC=gcc

C_FILES = $(wildcard src/*.c)
H_FILES = $(wildcard src/*.h)

default: debug release

$(TARGET)_dbg: $(C_FILES) $(H_FILES)
	@$(CC) -Wall -O0 -D DEBUG -g -pthread $^ -o $@

$(TARGET): $(C_FILES) $(H_FILES)
	@$(CC) -Wall -O3 $(CFLAGS) -pthread $^ -o $@

debug: $(TARGET)_dbg

release: $(TARGET)

clean:
	@rm -f $(TARGET)*
