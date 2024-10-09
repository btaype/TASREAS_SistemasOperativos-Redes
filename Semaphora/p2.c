#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#define KEY 0x1111

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

struct sembuf v = {0, 1, 0}; 
struct sembuf allow_terminal = {0, 0, 0};     

int main() {
    int id = semget(KEY, 1, 0666 | IPC_CREAT);
    if (id < 0) {
        perror("semget");
        exit(1);
    }

    
    union semun u;
    u.val = 0;
    if (semctl(id, 0, SETVAL, u) < 0) {
        perror("semctl");
        exit(1);
    }

    while (1) {
        if (semop(id, &allow_terminal, 1) < 0) {
                perror("semop allow");
                exit(2);
            }
        printf("Presiona 'v' para activar una terminal o 'q' para salir: ");
        char input;
        scanf(" %c", &input); 

        if (input == 'q') {
            printf("Saliendo del programa controlador...\n");
            break;
        }

        if (input == 'v') {
            
            printf("Activando una terminal...\n");
            
            if (semop(id, &v, 1) < 0) {
                perror("semop allow");
                exit(2);
            }
            /
            
        }
    }

    return 0;
}