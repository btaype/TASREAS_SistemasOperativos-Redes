/* Server code in C */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <map>
#include <iostream> // std::cout
#include <thread>   // std::thread, std::this_thread::sleep_for

#include <list>

std::list<int> listSockets;

using namespace std;
int idbasico=0;
map <string,int> nickname;
void writeSocketThread(int cliSocket)
{
  char buffer[300];
  int tamano;
  do
  {
    printf("Enter a msg:");
    fgets(buffer, 100, stdin);
    tamano = strlen(buffer);
    buffer[tamano] = '\0';
    tamano = write(cliSocket, buffer, tamano);

  } while (strncmp(buffer, "exit", 4) != 0);
  shutdown(cliSocket, SHUT_RDWR);
  close(cliSocket);
}
void readSocketThread(int cliSocket,string minombre)
{
  char buffer[201];
  char salir[4];
  bool deten=1;
  while(deten){

    int n = read(cliSocket , buffer, 5);
    buffer[n] = '\0';
    //memcpy(salir, buffer, 4); 
    //string nombre=string(buffer,n);
    printf("hollaaa%s\n",buffer);
    int tamano = atoi(buffer);
    
    
    n = read(cliSocket, buffer, 1);
    printf("%c\n",buffer[0]);

    if (buffer[0]=='L'){
        
        buffer[0] = '\0';   

        for (const auto& par : nickname) {
            strcat(buffer, par.first.c_str());  
            strcat(buffer, "\n");               
        }
      char buffer_2[250];
      sprintf(buffer_2,"%05dl%s",strlen(buffer)+1,buffer);
      write(cliSocket,buffer_2,strlen(buffer)+1+5);
      

    }
    else if(buffer[0]=='M'){
     // printf("si entre\n");


      n = read(cliSocket , buffer, 5);

      buffer[n] = '\0';
      //printf("buffer (tamano): %s\n", buffer);

      tamano = atoi(buffer);
      //printf("tamano: %d\n", tamano);


      n = read(cliSocket, buffer, tamano);

      buffer[n] = '\0';
      //printf("mensaje 1: %s\n", buffer);

      // leer tamaño 2
      char buffer2[101];
      n = read(cliSocket, buffer2, 5);

      buffer2[n] = '\0';
      //printf("buffer2 (tamano2): %s\n", buffer2);

      int tamano2 = atoi(buffer2);
      //printf("tamano2: %d\n", tamano2);

      n = read(cliSocket, buffer2, tamano2);

      buffer2[n] = '\0';
      //printf("mensaje 2: %s\n", buffer2);


      char envio[301];
      sprintf(envio, "%05dm%05d%s%05d%s", 
              tamano + minombre.size() + 1, 
              tamano, buffer, 
              (int)minombre.size(), 
              minombre.c_str());
      if((nickname.count(buffer2) > 0)){
        printf("\nenvio final: %s\n", envio); 

        write(nickname[buffer2],envio,tamano+minombre.size()+1+5+5+5);



      }
      else{ 

          printf("\n No existe el usuario \n"); 
      }
      


    }
    else if(buffer[0]=='B'){
      n=read(cliSocket,buffer,5);
      buffer[n]='\0';
      tamano = atoi(buffer);
      n=read(cliSocket,buffer,tamano);
      char  envio[201];
      sprintf(envio,"%05db%05d%s%05d%s",(int)minombre.size()+tamano+11,tamano,buffer,(int)minombre.size(),minombre.c_str());
      for (const auto& par : nickname) {
        if (minombre!=par.first){
          write(par.second,envio,(int)minombre.size()+tamano+11+5);
         }
      
      }
      


    }
    else if(buffer[0]=='E'){
      printf("\nSE RETIRO : %s",minombre.c_str());
      fflush(stdout);
      deten=0;
      //listSockets.remove(cliSocket);
      nickname.erase(minombre);
      shutdown(cliSocket, SHUT_RDWR);
      close(cliSocket);
      break;
     

    }

  } 
  
}

int main(void)
{
  struct sockaddr_in stSockAddr;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  char buffer[256];
  int n;

  if (-1 == SocketFD)
  {
    perror("can not create socket");
    exit(EXIT_FAILURE);
  }

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(4000);
  stSockAddr.sin_addr.s_addr = INADDR_ANY;

  if (-1 == bind(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
  {
    perror("error bind failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  if (-1 == listen(SocketFD, 10))
  {
    perror("error listen failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }

  for (;;)
  {
    int ClientFD = accept(SocketFD, NULL, NULL);

    if (0 > ClientFD)
    {
      perror("error accept failed");
      close(SocketFD);
      exit(EXIT_FAILURE);
    }
    //listSockets.push_back(ClientFD);
    
    int n = read(ClientFD , buffer, 5);
    buffer[n] = '\0';
    
    //string nombre=string(buffer,n);
    //printf("%s\n",buffer);
    int tamano = atoi(buffer);
    
    
    n = read(ClientFD , buffer, 1);
    if (buffer[0]=='N'){
        n = read(ClientFD , buffer,tamano-1);
        buffer[n]='\0';
        printf("!!bienvenido!! --> ");
        printf("%s\n",buffer);

        nickname[buffer]=ClientFD;
        std::thread(readSocketThread, ClientFD,buffer).detach();


    }
    
    else{

        shutdown(ClientFD, SHUT_RDWR);
        close(ClientFD);
    }
    
    
    

    //std::thread(writeSocketThread, ClientFD).detach();

  }

  //close(SocketFD);
  return 0;
}
