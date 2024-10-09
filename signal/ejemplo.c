#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int signal) {
    if (signal == SIGUSR1) {
        printf("Señal SIGUSR1 recibida.\n");
    }
}

int main() {
    // Configurar el manejador de señales
    signal(SIGUSR1, signal_handler);

    printf("PID del proceso: %d\n", getpid());

    // Enviar la señal SIGUSR1 al propio proceso después de 2 segundos
    sleep(2);
    printf("Enviando señal SIGUSR1 al propio proceso...\n");
    kill(getpid(), SIGUSR1); // Enviar la señal al propio proceso

    // Esperar indefinidamente para que el proceso no termine inmediatamente
    while (1) {
        pause(); // Esperar señales
    }

    return 0;
}