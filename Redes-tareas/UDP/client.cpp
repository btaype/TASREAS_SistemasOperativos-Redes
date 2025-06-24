// udp_client.cpp
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include<iostream>

using namespace std;
#define PUERTO 8080
#define BUFFER_SIZE 500

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr;

    // Crear socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Fallo en socket");
        return 1;
    }

    char nombre[150];
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PUERTO);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP debe coincidir con la del servidor
    
    
    printf("nombre: ");
    fflush(stdout);
    
    fgets(nombre, 100, stdin);
    int tamano = strlen(nombre);
    int tamanototal= tamano+1;
    sprintf(buffer, "%05d%05d%05d%05dN%s",
                         0, 1, tamanototal + 5, tamanototal, nombre);
    
     int offset = tamanototal+20;
    for (int i = offset; i < BUFFER_SIZE; i++) {
        buffer[i] = '#';
    }

    // Enviar
    int enviado = sendto(sockfd, buffer, BUFFER_SIZE, 0,
                         (struct sockaddr *)&servaddr, sizeof(servaddr));


    while (true) {
        int opcion=0;    
        std::cout << "\n--- MENÚ ---\n";
        std::cout << "1. Ver lista de usuarios\n";
        std::cout << "2. Enviar mensaje\n";

        
        std::cout << "3. Enviar mensaje a todos\n";
        std::cout << "4. Enviar archivo a usuario\n";
        std::cout<<  "5. Join #\n";
        std::cout<<  "6. enviar posicion #\n";
        std::cout<<  "7. view #\n";
        std::cout << "8. Salir\n";
        
        std::cout << "Seleccione una opción: ";
        std::cin >> opcion;
        std::cin.ignore();  
        if (opcion == 1) {
           // list_users(SocketFD);
        }
        else if (opcion == 2) {
           //chatear(SocketFD);
        }
        else if (opcion == 3) {
            
            //mssjgeneral(SocketFD);
        }
        else if (opcion == 8) {
            //quit(SocketFD);
            break;
        }
        else if (opcion == 7) {
           // viewmichi(SocketFD);
        }
        else if (opcion == 4) {
            //enviarArchivo(SocketFD);
        }
        else if (opcion == 5) {
           //jugarmichi(SocketFD);
        }
        else if (opcion == 6) {
           //enviarposs(SocketFD);
        }

        else {
            std::cout << "Opción inválida. Intenta de nuevo.\n";
        }
    

    
    }
    close(sockfd);
    return 0;
}