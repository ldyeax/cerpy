CC      := gcc
CFLAGS  := -Wall -Wextra -Wno-cpp -std=c89 -pedantic -D_FORTIFY_SOURCE=2 -O1 -g
LDFLAGS := 
OBJECTS := parse.o linebuf.o conn.o derpy.o
BINARY  := cerpy

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	rm -f $(OBJECTS)
	rm -f $(BINARY)
