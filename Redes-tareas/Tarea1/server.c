  /* Server code in C */
 
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
        printf("Cliente enmvio: %s\n",buffer2);
        
        if (strcmp(buffer2, "chau") == 0) {
            printf("Cliente dijo chau → cerrando conexion\n");
            break;
        }
      }

    shutdown(Csocket, SHUT_RDWR);
    close(Csocket);
    

    salir=0;
    
    return NULL;

 }
 void* writes(void* arg) {
    int Csocket = *(int*)arg;
    char texto[256];
    while (salir) {
       fgets(texto, 255, stdin);
        int tamano = strlen(texto);
        texto[tamano - 1] = '\0'; 
        tamano = strlen(texto);
        write(Csocket, texto, tamano);
        if (strcmp(texto, "chau") == 0) {
            printf("server dijo chau → cerrando conexion\n");
            
            break;
        }
    }
    salir = 0;
    return ;
}
  int main(void)
  {
    struct sockaddr_in stSockAddr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    char buffer[256];
    int n;
 
    if(-1 == SocketFD)
    {
      perror("can not create socket");
      exit(EXIT_FAILURE);
    }
 
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
 
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(1100);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;
 
    if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
      perror("error bind failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    if(-1 == listen(SocketFD, 10))
    {
      perror("error listen failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
 
    for(;;)
    {
      int ConnectFD = accept(SocketFD, NULL, NULL);
 
      if(0 > ConnectFD)
      {
        perror("error accept failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
      }
      pthread_t lee,rt;
         
      salir=1;
        pthread_create(&lee, NULL, reads, &ConnectFD);
        pthread_create(&rt, NULL, writes, &ConnectFD);

        pthread_join(lee, NULL);      
        pthread_cancel(rt);          
        pthread_join(rt, NULL);   
     
 
      shutdown(ConnectFD, SHUT_RDWR);
 
      close(ConnectFD);
    }
 
    close(SocketFD);
    return 0;
  }
