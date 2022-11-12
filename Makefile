CC = gcc

TARGETS = severM severA severB severC clientA clientB 

CFLAGS = -g -Wall

all: $(TARGETS)

clean:
	$(RM) $(TARGETS)


%: %.c
	$(CC) $(CFLAGS) -o $@ $<