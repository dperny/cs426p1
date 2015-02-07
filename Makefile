CC=gcc
CFLAGS=-Wall
EXECUTABLE=osh

all: $(EXECUTABLE)

$(EXECUTABLE): osh.c
	$(CC) $(CFLAGS) -o $@ $<

clean: 
	rm $(EXECUTABLE)
