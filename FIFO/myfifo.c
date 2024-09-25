#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
    int fd[2];
    int fd1[2];
    pid_t childpid;
    
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }
     if (pipe(fd1) == -1) {
        perror("pipe");
        exit(1);
    }
    if ((childpid = fork()) == -1) {
        perror("fork");
        exit(1);
    }

    if (childpid == 0) {
        int contador;
        FILE *archivo;
        archivo = fopen("tex1.txt", "r");
        if (archivo == NULL) {
            perror("Error opening tex1.txt");
            exit(1);
        }
        
        printf("Child program\n");
        close(fd[0]); 
        
        char buffer[100];
        while (fgets(buffer, sizeof(buffer), archivo) != NULL) {
            write(fd[1], buffer, strlen(buffer));
            printf("soy el hijo, envio: %s\n", buffer);  
            sleep(5);
        }
        
        fclose(archivo);
        close(fd[1]);
        close(fd1[1]);
        read(fd1[0], &contador, sizeof(contador));
        printf("soy el hijo, recibi: %d", contador); 
         close(fd1[0]);
        exit(0);
    } else {
        int contador=0;
        FILE *archivo;
        archivo = fopen("tex2.txt", "w");
        if (archivo == NULL) {
            perror("Error opening tex2.txt");
            exit(1);
        }
        
        printf("Parent program:\n");
        close(fd[1]); 
        
        char buffer[100];
        ssize_t nbytes;
        while ((nbytes = read(fd[0], buffer, sizeof(buffer))) > 0) {
            buffer[nbytes] = '\0'; 
            printf("Soy el padre, recibi: %s\n", buffer);  
            fputs(buffer, archivo);  
            sleep(5);
            contador++;
        }

        
        if (nbytes == -1) {
            perror("read");
        }
        
        close(fd[0]);  

        close(fd1[0]);
        write(fd1[1], &contador, sizeof(contador));
        printf("Soy el padre, envio: %d\n", contador);  
        close(fd1[1]);
        
        fclose(archivo);
        exit(0);
    }

    return 0;  
}