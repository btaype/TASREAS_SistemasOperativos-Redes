#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <errno.h>
#include <unistd.h>

#define QUEUE_NAME "/my_queue"
#define MAX_SIZE sizeof(int) // Tamaño máximo del mensaje (tamaño de un entero)
#define QUEUE_SIZE 10        // Número máximo de mensajes en la cola

int main() {
    int signal[] = {2, 10, 12, 18, 19};
    int interp[] = {0, 0, 0, 1, 1};
    mqd_t mq;
    struct mq_attr attr;

    // Configurar atributos de la cola
    attr.mq_flags = 0;            // Flags (no se utilizan)
    attr.mq_maxmsg = QUEUE_SIZE;  // Número máximo de mensajes
    attr.mq_msgsize = MAX_SIZE;    // Tamaño máximo del mensaje (un entero)
    attr.mq_curmsgs = 0;           // Mensajes actuales en la cola
    
    // Crear o abrir la cola de mensajes
    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    // Verificar atributos de la cola
    if (mq_getattr(mq, &attr) == -1) {
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }

    int opcion;
    int pid;

    printf("Seleccione una señal para enviar:\n");
    printf("1) SIGINT (Signal 2)\n");
    printf("2) SIGUSR1 (Signal 10)\n");
    printf("3) SIGUSR2 (Signal 12)\n");
    printf("4) SIGCONT (Signal 18)\n");
    printf("5) SIGSTOP (Signal 19)\n");
    printf("0) Salir\n");

    scanf("%d", &opcion);

    // Validar la opción seleccionada
    if (opcion < 0 || opcion > 5) {
        fprintf(stderr, "Opción no válida.\n");
        mq_close(mq);
        return 1; // Salir con un error
    }

    // Salir si se selecciona 0
    if (opcion == 0) {
        printf("Saliendo del programa.\n");
        mq_close(mq);
        return 0;
    }

    printf("Ingrese el PID del proceso al que desea enviar la señal: ");
    scanf("%d", &pid);
    int opcion_real = signal[opcion - 1];
    int interr = interp[opcion - 1];
    int t[] = {interr, opcion_real, pid};

    // Enviar los valores del array t uno por uno
    for (int i = 0; i < 3; i++) {
        // Asegúrate de que el mensaje no exceda el tamaño permitido
        if (sizeof(int) > MAX_SIZE) {
            fprintf(stderr, "El mensaje excede el tamaño máximo permitido\n");
            continue;
        }

        // Enviar el entero
        if (mq_send(mq, (char*)&t[i], sizeof(int), 0) == -1) {
            perror("mq_send");
            exit(EXIT_FAILURE);
        }
        printf("Enviado: %d\n", t[i]);
    }

    // Cerrar la cola de mensajes
    mq_close(mq);
    return 0;
}
