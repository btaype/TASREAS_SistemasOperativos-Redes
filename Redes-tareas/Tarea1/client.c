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
  int salir=1;
void reads(void*arg){

  int Csocket = *(int*)arg;
  char buffer2[256];
   while (1) {
        bzero(buffer2,256);
        int n = read(Csocket , buffer2, 255);
        if (n<=0){
          break;
        }
         buffer2[n] = '\0';
        printf("server envio: %s\n",buffer2);
        
        if (strcmp(buffer2, "chau") == 0) {
            printf("server dijo chau → cerrando conexion\n");
            break;
        }
      }

    shutdown(Csocket, SHUT_RDWR);
    close(Csocket);
    
    salir=0;
     fclose(stdin);
    return ;

 }
  int main(void)
  {
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    int n;
    
    if (-1 == SocketFD)
    {
      perror("cannot create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);
 
    if (0 > Res)
    {
      perror("error: first parameter is not a valid address family");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
      perror("char string (second parameter does not contain valid ipaddress");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("connect failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    pthread_t lee;
     pthread_create(&lee, NULL,reads,&SocketFD); 
    pthread_detach(lee);   
    salir=1;
    while(salir)
    {
    char texto[256];
    
    
    
    printf("enviar: ");
    
    
    fgets(texto, 255, stdin);
    int tamano = strlen(texto);
    texto[tamano - 1] = '\0'; 
    tamano = strlen(texto);

    n = write(SocketFD,texto,tamano );
     if (strcmp(texto, "chau") == 0) {
            printf("Cliente dijo chau → cerrando conexion\n");
            break;
        }
  } 
    
    shutdown(SocketFD, SHUT_RDWR);
    
    close(SocketFD);
    
    return 0;
    
  }
