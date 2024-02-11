CC := gcc
CFLAGS := -Wall -Wextra -Iinc -ggdb

SRCS := $(wildcard src/*.c)
OBJS := $(subst src/,build/,$(SRCS:.c=.o))

.PHONY: all clean

all: detector

clean:
	rm -rf build detector

$(OBJS): build/%.o: src/%.c
	mkdir -p $(dir $@) && $(CC) $(CFLAGS) -o $@ -c $^

detector: $(OBJS)
	$(CC) -o $@ $^
