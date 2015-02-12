CC=gcc
CFLAGS=-Wall -g
EXECUTABLE=osh

all: $(EXECUTABLE)

$(EXECUTABLE): osh.c
	$(CC) $(CFLAGS) -o $@ $<

clean: 
	rm $(EXECUTABLE)
