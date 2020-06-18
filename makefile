#	Group 47
#  2016B3A70562P   Akash Kabra
#  2016B3A70523P   Ayush Mungad
#  2016B4A70520P   Kumar Anant Raj
#  2016B4A70533P   Mustansir Mama
#  2016B5A70715P   Bhavesh Chand

CC = gcc 

#gcc
CFLAGS1 = -std=c99

#debug
CFLAGS2 = -g $(CFLAGS1)

#Library flags
LFLAGS = 

CFILES = $(wildcard *.c) 
COBJS = $(CFILES:.c=.o)

%.o: ./%.c
	$(CC) $(CFLAGS1) -c $<

all: $(COBJS)
	$(CC) $(CFLAGS1) -o compiler $(COBJS) $(LFLAGS)

run:
	nasm -felf64 code.asm
	gcc -no-pie -o exec code.o
	./exec
	
debug:
	$(CC) $(CFLAGS2) -o compiler $(CFILES) $(LFLAGS)
	gdb ./compiler
	
clean:
	rm -f $(COBJS)
	rm -f compiler
	rm -f code.asm
	rm -f code.o
	rm -f exec
