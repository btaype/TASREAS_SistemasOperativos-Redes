#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

#define QUEUE_NAME "/my_queue"
#define MAX_SIZE sizeof(int) // Tamaño máximo del mensaje
int inter = 0; // Interruptor para controlar el estado de recepción
int mens = 0;  // Interruptor para el estado de mensajes

mqd_t mq; // Descriptor de la cola de mensajes global

// Función para el primer hilo
void* receive_messages_thread1() {
    int msg;

    printf("Hilo 1 listo para recibir mensajes...\n");
    
    // Recibir mensajes en un bucle infinito
    while (1) {
        if (inter == 1) {
            sleep(2); // Esperar si 'inter' es 1
            continue; // Volver a comprobar el bucle
        }
        
        if (mens == 0) { // Estado inicial para recibir mensajes de control
            ssize_t bytes_read = mq_receive(mq, (char*)&msg, MAX_SIZE, NULL);
            if (bytes_read >= 0) {
                if (msg == 0) {
                    mens = 1; // Cambiar el estado a recibir mensajes
                } else if (msg == 1) {
                    inter = 1; // Cambiar a modo inactivo
                    mens = 1; // Cambiar el estado a recibir mensajes
                    continue;
                }
            } else {
                perror("mq_receive en hilo 1");
                break; // Salir del bucle si hay un error
            }
        }

        if (mens == 1) { // Procesar los mensajes cuando 'mens' es 1
            int r[2]; // Arreglo para almacenar dos mensajes

            for (int i = 0; i < 2; i++) {
                ssize_t bytes_read = mq_receive(mq, (char*)&msg, MAX_SIZE, NULL);
                if (bytes_read >= 0) {
                    r[i] = msg; // Almacenar el mensaje recibido
                } else {
                    perror("mq_receive");
                    exit(EXIT_FAILURE);
                }
            }
            mens = 0; // Reiniciar el estado de 'mens' para la próxima iteración
            // Aquí puedes procesar el arreglo 'r' como necesites
            printf("Hilo 1 procesó: %d y %d\n", r[0], r[1]); // Ejemplo de procesamiento
        }
    }
    return NULL;
}

// Función para el segundo hilo
void* receive_messages_thread2() {
    int msg;

    printf("Hilo 2 listo para recibir mensajes...\n");
    
    // Recibir mensajes en un bucle infinito
    while (1) {
        if (inter == 0) {
            sleep(2); // Esperar si 'inter' es 1
            continue; // Volver a comprobar el bucle
        }
        
        if (mens == 0) { // Estado inicial para recibir mensajes de control
            ssize_t bytes_read = mq_receive(mq, (char*)&msg, MAX_SIZE, NULL);
            if (bytes_read >= 0) {
                if (msg == 1) {
                    mens = 1; // Cambiar el estado a recibir mensajes
                } else if (msg == 0) {
                    inter = 0; // Cambiar a modo inactivo
                    mens = 1; // Cambiar el estado a recibir mensajes
                    continue;
                }
            } else {
                perror("mq_receive en hilo 2");
                break; // Salir del bucle si hay un error
            }
        }

        if (mens == 1) { // Procesar los mensajes cuando 'mens' es 1
            int r[2]; // Arreglo para almacenar dos mensajes

            for (int i = 0; i < 2; i++) {
                ssize_t bytes_read = mq_receive(mq, (char*)&msg, MAX_SIZE, NULL);
                if (bytes_read >= 0) {
                    r[i] = msg; // Almacenar el mensaje recibido
                } else {
                    perror("mq_receive");
                    exit(EXIT_FAILURE);
                }
            }
            mens = 0; // Reiniciar el estado de 'mens' para la próxima iteración
            // Aquí puedes procesar el arreglo 'r' como necesites
            printf("Hilo 2 procesó: %d y %d\n", r[0], r[1]); // Ejemplo de procesamiento
        }
    }
    return NULL;
}

int main() {
    // Abrir la cola de mensajes
    mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    pthread_t thread1, thread2; // Identificadores de los hilos

    // Crear dos hilos para recibir mensajes
    pthread_create(&thread1, NULL, receive_messages_thread1, NULL);
    pthread_create(&thread2, NULL, receive_messages_thread2, NULL);

    // Esperar a que los hilos terminen (esto nunca sucederá en este caso)
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Cerrar y eliminar la cola de mensajes (nunca alcanzará aquí en este ejemplo)
    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    return 0;
}
