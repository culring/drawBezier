CC = gcc
CFLAGS = -Wall -m64

all: main.o bezier.o
	$(CC) $(CFLAGS) main.o bezier.o -o main -lglut -lGL -lGLU -lm

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

bezier.o: bezier.asm
	nasm -f elf64 bezier.asm -o bezier.o

clean:
	if [ -e main ]; then rm -f main; fi
	rm -f *.o
