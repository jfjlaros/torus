CC = gcc
CFLAGS = -O3 -Wall
LFLAGS = -lglut -lm -lGL

EXEC = torus
MAIN = torus.c
OBJS = input.o output.o

all: $(EXEC)

.cc.o:
	$(CC) $(CFLAGS) -c $<
	
$(EXEC): $(MAIN) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

release: $(EXEC) clean
	strip $(EXEC)

clean:
	rm -f $(OBJS) core

distclean: clean
	rm -f $(EXEC)
