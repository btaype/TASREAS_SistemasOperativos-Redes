# Regla principal
all: main.dynamic.exe

# Compilar los archivos .c en archivos objeto con -fPIC
libucsp.so: *.c
	gcc -c -fPIC *.c
	gcc -shared -o libucsp.so *.o

# Compilar el programa principal y enlazar con la biblioteca dinámica
main.dynamic.exe: libucsp.so main.c
	gcc main.c -L. -lucsp -o main.dynamic.exe

# Ejecutar el programa, asegurando que la biblioteca dinámica esté en LD_LIBRARY_PATH
run: all
	LD_LIBRARY_PATH=.:$$LD_LIBRARY_PATH ./main.dynamic.exe

# Limpiar los archivos generados
clean:
	rm -f *.o libucsp.so main.dynamic.exe