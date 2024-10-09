#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Funciones para manejar señales
void manejar_signal_2(int sig) {
    printf("Señal 2 (SIGINT) recibida.\n");
}

void manejar_signal_10(int sig) {
    printf("Señal 10 (SIGUSR1) recibida.\n");
}

void manejar_signal_12(int sig) {
    printf("Señal 12 (SIGUSR2) recibida.\n");
}

void manejar_signal_18(int sig) {
    printf("Señal 18 recibida.\n");
}

int main() {
    // Configurar manejadores para señales específicas
    signal(2, manejar_signal_2);  // SIGINT
    signal(10, manejar_signal_10); // SIGUSR1
    signal(12, manejar_signal_12); // SIGUSR2
    signal(18, manejar_signal_18); // Señal ficticia 18

    // Imprimir el ID de proceso
    printf("Esperando señales... (PID: %d)\n", getpid());

    // Bucle infinito para mantener el programa en ejecución
    while (1) {
        pause(); // Esperar por una señal
    }

    return 0;
}