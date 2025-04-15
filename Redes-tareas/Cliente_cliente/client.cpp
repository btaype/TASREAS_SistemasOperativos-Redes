/* Client code in C */

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
bool controlmsg = 1;
const char* para = "para";
const char* msgg = "msg";

void* readSocketThread(void* arg) {
    int cliSocket = *(int*)arg;
    char buffer[300];
    int n;

    while (1) {
        n = read(cliSocket , buffer, 5);
        
        if (n <= 0) {
            printf("\n[Cliente] Conexión cerrada o error. Cerrando hilo...\n");
            break;
        }
       // printf("entardnobucle");
        buffer[n] = '\0';
        
        int tamano = atoi(buffer);
        
        n = read(cliSocket, buffer, 1);
        if (n <= 0) break;

        if (buffer[0] == 'l') {
            buffer[0] = '\0';   
            n = read(cliSocket, buffer, tamano - 1);
            if (n <= 0) break;
            buffer[n] = '\0';
            printf("\nLista de usuarios:\n%s\n", buffer);
        }
        else if (buffer[0] == 'm') {
            char buffer2[100];

            n = read(cliSocket, buffer, 5);
            if (n <= 0) break;
            buffer[n] = '\0';
            tamano = atoi(buffer);

            n = read(cliSocket, buffer, tamano);
            if (n <= 0) break;
            buffer[n] = '\0';

            n = read(cliSocket, buffer2, 5);
            if (n <= 0) break;
            buffer2[n] = '\0';
            tamano = atoi(buffer2);

            n = read(cliSocket, buffer2, tamano);
            if (n <= 0) break;
            buffer2[n] = '\0';

            printf("\nMensaje de <%s>: %s\n", buffer2, buffer);
        }
        else if (buffer[0] == 'b') {
            n = read(cliSocket, buffer, 5);
            if (n <= 0) break;
            buffer[n] = '\0';
            tamano = atoi(buffer);

            n = read(cliSocket, buffer, tamano);
            if (n <= 0) break;
            buffer[n] = '\0';

            char nombre_p[101];
            n = read(cliSocket, nombre_p, 5);
            if (n <= 0) break;
            nombre_p[n] = '\0';
            tamano = atoi(nombre_p);

            n = read(cliSocket, nombre_p, tamano);
            if (n <= 0) break;
            nombre_p[n] = '\0';

         
        }
        else if (buffer[0] == 'f') {
            
            
            n = read(cliSocket, buffer, 5);
            buffer[n] = '\0';
            int len_dest = atoi(buffer);

            
            char dummy_dest[101];
            n = read(cliSocket, dummy_dest, len_dest);
            dummy_dest[n] = '\0';

        
            n = read(cliSocket, buffer, 5);
            buffer[n] = '\0';
            int len_nombre = atoi(buffer);

        
            char nombre_archivo[101];
            n = read(cliSocket, nombre_archivo, len_nombre);
            nombre_archivo[n] = '\0';

            
            char buffer_size[19];
            n = read(cliSocket, buffer_size, 18);
            buffer_size[n] = '\0';
            long file_size = atol(buffer_size);

            
            char* contenido = (char*)malloc(file_size);
            n = read(cliSocket, contenido, file_size);
            char nuevo_nombre[256];


            char* punto = strrchr(nombre_archivo, '.');

            if (punto) {
               
                size_t len_base = punto - nombre_archivo;
                strncpy(nuevo_nombre, nombre_archivo, len_base);
                nuevo_nombre[len_base] = '\0';  

                strcat(nuevo_nombre, "copia");
                strcat(nuevo_nombre, punto);   
            } else {
                
                sprintf(nuevo_nombre, "%scopia", nombre_archivo);
}
           
            FILE* f = fopen(nuevo_nombre, "wb");
            if (!f) {
                perror("No se pudo crear el archivo");
                free(contenido);
                return NULL;
            }
            fwrite(contenido, 1, file_size, f);
            fclose(f);
            free(contenido);

            printf("\nArchivo recibido de <%s> y guardado como '%s' (%ld bytes).\n",
                dummy_dest, nombre_archivo, file_size);
        }

    }

    shutdown(cliSocket, SHUT_RDWR);
    close(cliSocket);
    return NULL;
}
void list_users(int soketfd){
    char buffer_temp[8]="00001L\0";
    write(soketfd,buffer_temp,6);
    return;
}
void chatear(int soketfd){;
    char buffer[101];
    char buffer2[201];
    char buffer3[301];

    printf("%s : ", para);
        fflush(stdout);
        fgets(buffer, 100, stdin);
        int tamano1 = strlen(buffer);
        buffer[tamano1 - 1] = '\0'; 
        //printf("%d\n", n);
       
        printf("%s : ", msgg);
        fflush(stdout);
        fgets(buffer2, 200, stdin);
        int tamano2= strlen(buffer2);
        buffer2[tamano2 - 1] = '\0';
        //aqui el tamano es tamano-1 porque en el la cadena de char lo toma como \n el ultimo caracter;

        sprintf(buffer3,"%05dM%05d%s%05d%s",tamano1+tamano2-1,tamano2-1,buffer2,tamano1-1,buffer);
        printf("\n%s\n",buffer3);
        write(soketfd, buffer3, tamano1+tamano2+5+4+5);
        

    return;
}

 void quit(int soketfd){
    char buffer_temp[8]="00001E\0";
   write(soketfd,buffer_temp,6);  
    std::cout << "Saliendo del chat...\n";


 }
    
void mssjgeneral(int soketfd){
    char buffer[101];
    
    char buffer3[301];

    printf("mensaje : ");
    fflush(stdout);
    fgets(buffer, 100, stdin);
    int tamano1 = strlen(buffer);
    buffer[tamano1 - 1] = '\0'; 
    sprintf(buffer3,"%05dB%05d%s",tamano1+5,tamano1-1,buffer);
    write(soketfd,buffer3,tamano1+5+5);
 }

void enviarArchivo(int socketfd) {
    char buffer[101];    // Para destino
    char buffer2[201];   // Para nombre del archivo

    // Pedir DESTINO
    printf("Destino : ");
    fflush(stdout);
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Eliminar \n

    // Pedir NOMBRE DEL ARCHIVO
    printf("Archivo a enviar : ");
    fflush(stdout);
    fgets(buffer2, sizeof(buffer2), stdin);
    buffer2[strcspn(buffer2, "\n")] = '\0'; // Eliminar \n

    const char* destino = buffer;
    const char* nombreArchivo = buffer2;

    FILE* file = fopen(nombreArchivo, "rb");
    if (!file) {
        perror("No se pudo abrir el archivo");
        return;
    }

    // Obtener tamaño del archivo
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Leer archivo en memoria
    char* file_buffer = (char*)malloc(file_size + 1);
    if (!file_buffer) {
        perror("No se pudo reservar memoria");
        fclose(file);
        return;
    }
    fread(file_buffer, 1, file_size, file);
    fclose(file);
    file_buffer[file_size] = '\0'; // Por seguridad (aunque no sea texto)

    // Calcular longitudes
    int len_destino = strlen(destino);
    int len_nombre = strlen(nombreArchivo);

    // Crear header
    char header[1024] = "";
    char temp[64];

    sprintf(temp, "%05d", len_destino);
    strcat(header, temp);
    strcat(header, destino);

    sprintf(temp, "%05d", len_nombre);
    strcat(header, temp);
    strcat(header, nombreArchivo);

    sprintf(temp, "%018ld", file_size);
    strcat(header, temp);

    // Ahora calcular total size del mensaje
    int total_size = 5 + 1 + strlen(header) + file_size;

    // Crear mensaje final
    char* mensaje = (char*)malloc(total_size + 1);
    if (!mensaje) {
        perror("No se pudo reservar memoria para mensaje");
        free(file_buffer);
        return;
    }

    // Armar mensaje completo
    sprintf(mensaje, "%05d", total_size);
    mensaje[5] = 'F'; // Tipo de mensaje
    mensaje[6] = '\0';

    strcat(mensaje, header);
    memcpy(mensaje + strlen(mensaje), file_buffer, file_size); // Solo aquí usamos memcpy por binarios

    // Enviar mensaje
    write(socketfd, mensaje, total_size);
    printf("Archivo enviado correctamente (%s, %ld bytes).\n", nombreArchivo, file_size);

    // Liberar memoria
    free(file_buffer);
    free(mensaje);
}

int main(void) {
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;
    char buffer[250];

    if (-1 == SocketFD) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(4000);
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);

    if (0 > Res) {
        perror("error: first parameter is not a valid address family");
        close(SocketFD);
        exit(EXIT_FAILURE);
    } else if (0 == Res) {
        perror("char string (second parameter does not contain valid ipaddress");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in))) {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    char nombre[150];
    pthread_t thread;
    
    
    printf("nombre: ");
    fflush(stdout);
    
    fgets(nombre, 100, stdin);
    int tamano = strlen(nombre);
    nombre[tamano - 1] = '\0'; 
    tamano = strlen(nombre);
    sprintf(buffer,"%05dN%s",tamano+1,nombre);
    
    //cout<<tamano<<'\n';
    write(SocketFD,buffer, tamano+1+5);
    pthread_create(&thread, NULL, readSocketThread, &SocketFD);
    pthread_detach(thread);


        while (true) {
        int opcion=0;    
        std::cout << "\n--- MENÚ ---\n";
        std::cout << "1. Ver lista de usuarios\n";
        std::cout << "2. Enviar mensaje\n";

        
        std::cout << "3. Enviar mensaje a todos\n";
        std::cout << "4. Enviar archivo a usuario\n";
        std::cout << "5. Salir\n";
        
        std::cout << "Seleccione una opción: ";
        
        std::cin >> opcion;
        std::cin.ignore();  

        if (opcion == 1) {
            list_users(SocketFD);
        }
        else if (opcion == 2) {
           chatear(SocketFD);
        }
        else if (opcion == 3) {
            
            mssjgeneral(SocketFD);
        }
        else if (opcion == 5) {
            quit(SocketFD);
            break;
        }
        else if (opcion == 4) {
        enviarArchivo(SocketFD);
        }

        else {
            std::cout << "Opción inválida. Intenta de nuevo.\n";
        }
    }
    /*do {
        controlmsg = 1;
        printf("%s : ", para);
        fflush(stdout);
        fgets(buffer, 100, stdin);
        n = strlen(buffer);
        buffer[n - 1] = '\0'; // Eliminar el newline
        printf("%d\n", n);
        write(SocketFD, buffer, n-1);
        
        controlmsg = 0;
        printf("%s : ", msgg);
        fflush(stdout);
        fgets(buffer, 100, stdin);
        n = strlen(buffer);
        buffer[n - 1] = '\0'; // Eliminar el newline
        write(SocketFD, buffer, n-1);

    } while (strncmp(buffer, "exit", 4) != 0);
    */
    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
    return 0;
}