CC      := gcc
CFLAGS  := -Os -flto -Wall -Wextra -I../../include

SRC     := ../../src/ucli.c main.c
OBJ     := $(SRC:.c=.o)

TARGET  := main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
