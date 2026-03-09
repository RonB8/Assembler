
TARGET = assembler.out
CC = gcc
OPT = 
CFLAGS = -ansi -pedantic -Wall -g $(CPPFLAGS)
CFILES = $(wildcard *.c)
HFILES = $(wildcard *.h)
OBJECTS = $(patsubst %.c, %.o, $(CFILES))

.PHONY: all debug release clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^

$(OBJECTS): $(HFILES)

debug: $(OBJECTS)
	$(CC)  $(CFLAGS) $^ -o $(TARGET)

release: $(OBJECTS)
	$(CC) $(OPT) $^ -o $(TARGET)

clean:
	rm $(TARGET) $(OBJECTS) *.am *.ob *.ent *.ext
