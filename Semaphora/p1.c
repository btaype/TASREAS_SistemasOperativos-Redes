#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#define KEY 0x1111
#define N 15

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

struct sembuf p = {0, -1, 0};  
struct sembuf v = {0, 1, 0}; 

int main() {
    int id = semget(KEY, 1, 0666 | IPC_CREAT);
    if (id < 0) {
       
        exit(1);
    }

    printf("ID proceso:%d\n", getpid());

    
    while (1) {
        
        if (semop(id, &p, 1) < 0) {
        
            exit(2);
        }

      
        for (int i = 0; i < N; ++i) {
            printf("Línea : %d\n", i + 1);
            fflush(stdout);
            sleep(1); 
        }

        
        if (semop(id, &v, 1) < 0) {
            
            exit(3);
        }

        
        
    }

    return 0;
}