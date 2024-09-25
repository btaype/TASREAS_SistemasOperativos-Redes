
all: main.static.exe

libucsp.a: *.c
	gcc -c lib.c -o *.o
	ar rcs libucsp.a *.o


main.static.exe: main.c libucsp.a
	gcc main.c -L. -lucsp -o main.static.exe


run: all
	./main.static.exe


clean:
	rm -f *.o libucsp.a main.static.exe