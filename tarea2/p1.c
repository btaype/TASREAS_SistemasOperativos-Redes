#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Variable global para el modo de operación
int modo_verbose = 0;

// Manejador para SIGINT
void manejador_SIGINT(int sig) {
    // Alternar entre modo normal y verbose
    modo_verbose = !modo_verbose;
    if (modo_verbose) {
        printf("Modo cambiado a VERBOSE.\n");
    } else {
        printf("Modo cambiado a NORMAL.\n");
    }
}

int main() {
    // Configura el manejador de señales para SIGINT
    signal(SIGINT, manejador_SIGINT);

    printf("Programa en ejecución. PID: %d\n", getpid());

    // Bucle principal del programa
    while (1) {
        if (modo_verbose) {
            printf("Procesando datos en modo VERBOSE...\n");
        } else {
            printf("Procesando datos en modo NORMAL...\n");
        }
        sleep(2);  // Simula el procesamiento con una pausa de 2 segundos
    }

    return 0;
}