CC      := gcc
CFLAGS  := -Wall -Wextra -Wno-cpp -std=c89 -pedantic -D_FORTIFY_SOURCE=2 -O1 -g -Iinc/
LDFLAGS := 
OBJECTS := obj/parse.o obj/linebuf.o obj/conn.o obj/derpy.o
BINARY  := cerpy

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	rm -f $(OBJECTS)
	rm -f $(BINARY)
