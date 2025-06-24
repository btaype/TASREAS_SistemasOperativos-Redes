// udp_server.cpp
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include<iostream>
#include <unordered_map>
using namespace std;
#define PUERTO 8080
#define BUFFER_SIZE 500

std::unordered_map<std::string, sockaddr_in> nicknameMap;

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);

    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Fallo en socket");
        return 1;
    }

    
    const char* hostname = "127.0.0.1";
    struct hostent* host = gethostbyname(hostname);
    if (!host) {
        std::cerr << "No se pudo resolver el host: " << hostname << std::endl;
        return 1;
    }

    // Configurar dirección
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PUERTO);
    servaddr.sin_addr.s_addr = *(long*)host->h_addr; 

    // Enlazar socket
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Fallo en bind");
        return 1;
    }

    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                     (struct sockaddr*)&cliaddr, &len);
    if (n < 0) {
        perror("Error al recibir");
        return 1;
    }

    buffer[n] = '\0';

    
    char temp[6];
    strncpy(temp, buffer, 5);
    temp[5] = '\0';
    int tamano = atoi(temp);

    
    char tipo = buffer[5];

    if (tipo == 'N') {
        
        char nombre[150];
        strncpy(nombre, buffer + 6, tamano - 1);
        nombre[tamano - 1] = '\0';

        printf("!!Bienvenido!! --> %s\n", nombre);

       
        nicknameMap[nombre] = cliaddr;

        
    } else {
        printf("Mensaje desconocido.\n");
    }

    close(sockfd);
    return 0;
}
