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
#include <iostream> 
#include <thread>   
#include<vector>
#include <list>
#include <algorithm>
#include <chrono>
std::list<int> listSockets;


using namespace std;
int idbasico=0;
map <string,int> nickname;
map <string,char> jugadores;
vector<string> view;
vector <string> orden;
bool pos=0;
int cont_jugadas=0;
char  michi[9]={' ',' ',' ',' ',' ',' ',' ',' ',' '};
bool  verificaposs(int poss)
{ 
  if (michi[poss-1]==' '){
    return true;
  }
  return false;
}
bool hayGanador() {
    int lineas[8][3] = {
        {0, 1, 2}, 
        {3, 4, 5},
        {6, 7, 8},
        {0, 3, 6}, 
        {1, 4, 7},
        {2, 5, 8},
        {0, 4, 8}, 
        {2, 4, 6}
    };

    for (int i = 0; i < 8; ++i) {
        int a = lineas[i][0];
        int b = lineas[i][1];
        int c = lineas[i][2];

        if (michi[a] != ' ' && michi[a] == michi[b] && michi[b] == michi[c]) {
            return true;
        }
    }
    return false;
}
void limpiar(){
  cont_jugadas=0;
            pos=0;
            orden.clear();
            jugadores.clear();
            view.clear();

}
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
void procesarMensaje(int cliSocket, string minombre, char tipo)
{
  char buffer[201];
  char salir[4];
  bool deten=1;
    int n;
    int tamano ;
    if (tipo == 'L'){
        
        buffer[0] = '\0';   

        for (const auto& par : nickname) {
            strcat(buffer, par.first.c_str());  
            strcat(buffer, "\n");               
        }
      char buffer_2[250];
      sprintf(buffer_2,"%05dl%s",strlen(buffer)+1,buffer);
      write(cliSocket,buffer_2,strlen(buffer)+1+5);
      

    }
    else if(tipo == 'M'){
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
    else if(tipo =='B'){
      n=read(cliSocket,buffer,5);
      buffer[n]='\0';
      tamano = atoi(buffer);
      n=read(cliSocket,buffer,tamano);
      buffer[n]='\0';
      char  envio[201];
      sprintf(envio,"%05db%05d%s%05d%s",(int)minombre.size()+tamano+11,tamano,buffer,(int)minombre.size(),minombre.c_str());

      for (const auto& par : nickname) {
        if (minombre!=par.first){
          
          write(par.second,envio,(int)minombre.size()+tamano+11+5);
           printf("\n%s\n",envio);
            
         }
      
      }
      


    }
   
    else if(tipo =='J'){
      if (jugadores.size()==0){
        jugadores[minombre]='O';
        char buffer2[101];
        string msg1="Wait for please";
        string serverm="Server TicTacToe";
        sprintf(buffer2,"%05dm%05d%s%05d%s",1+serverm.size()+msg1.size(),msg1.size(),msg1.c_str(),serverm.size(),serverm.c_str());
        printf("%s\n",buffer2);
        write(nickname[minombre],buffer2,1+serverm.size()+msg1.size()+5+5+5);
        orden.push_back(minombre);
      }
      else if (jugadores.size()==1){
        //printf("2");
        jugadores[minombre]='X';
        char buffer2[101];
        string msg1="Iniciando";
        string serverm="Server TicTacToe";
        sprintf(buffer2,"%05dm%05d%s%05d%s",1+serverm.size()+msg1.size(),msg1.size(),msg1.c_str(),serverm.size(),serverm.c_str());
        printf("%s\n",buffer2);
        write(nickname[minombre],buffer2,1+serverm.size()+msg1.size()+5+5+5);
        orden.push_back(minombre);
        sort(orden.begin(),orden.end());
        char enviar[20];
        sprintf(enviar,"%05dT%c",2,jugadores[orden[pos]]);
        write(nickname[orden[pos]],enviar,7);
        //pos=!pos;
      }
      else {
        //printf("3");
        char buffer2[101];
         string msg1="Quieres ver?";
        string serverm="Server TicTacToe";
        sprintf(buffer2,"%05dm%05d%s%05d%s",1+serverm.size()+msg1.size(),msg1.size(),msg1.c_str(),serverm.size(),serverm.c_str());
        printf("%s\n",buffer2);
        write(nickname[minombre],buffer2,1+serverm.size()+msg1.size()+5+5+5);
        //view.push_back(minombre);
      }

    }
    else if(tipo =='V'){
      view.push_back(minombre);
    }
    else if (tipo =='P'){
      ///printf("\nCorrecto\n");

      char buffer[2]; 
      read(cliSocket, buffer, 1);
      buffer[1] = '\0';  

      int poss1 = atoi(buffer);

      char simbolo;
      read(cliSocket, &simbolo, 1);

     if (minombre==orden[pos]){
        if (verificaposs(poss1)) {
          michi[poss1 - 1] = simbolo;
          cont_jugadas=cont_jugadas+1;
          char michiencia[20];
          sprintf(michiencia,"%05dX%s",10,michi);
          for (size_t i=0;i<orden.size();i++){
            write(nickname[orden[i]],michiencia,15);
          }
          for (size_t i=0;i<view.size();i++){
            write(nickname[view[i]],michiencia,15);
          }
          bool ganador=hayGanador();
          if (ganador){
            char win[10];
            char loss[10];
            
            sprintf(win,"%05dOW",2);
            sprintf(loss,"%05dOL",2);
            write(nickname[orden[pos]],win,7);
            write(nickname[orden[!pos]],loss,7);
            printf("\n%s\n",win);
            printf("\n%s\n",loss);
            limpiar();
          }
          else if (cont_jugadas==9){
            char win[10];
            char loss[10];
            
            sprintf(win,"%05dOL",2);
            sprintf(loss,"%05dOL",2);
            write(nickname[orden[pos]],win,7);
            write(nickname[orden[!pos]],loss,7);
            limpiar();
          }
          else {
          pos=!pos;
          char enviar[20];
          sprintf(enviar,"%05dT%c",2,jugadores[orden[pos]]);
          write(nickname[orden[pos]],enviar,7);
          }
          
          
      } else {
          //printf("\nOcupado\n");
          string error="POSICION OCUPADA";
          int nerror=6;
          char enviar[20];
          sprintf(enviar,"%05dE%01d%05d%s",error.size()+1+1+5,nerror,error.size(),error.c_str());


          write(cliSocket,enviar,error.size()+1+1+5+5);

      }

     }
     else {
      //printf("\nno te toca\n");
      
     }
      













    }
    else if (tipo =='F'){
    printf("\n--- INICIO DE RECEPCIÓN DE ARCHIVO ---\n");

    // 1. Longitud del nombre destino
    n = read(cliSocket, buffer, 5);
    buffer[n] = '\0';
    int len_dest = atoi(buffer);
    printf("1. Recibí longitud de destino: '%s' (%d bytes)\n", buffer, n);

    // 2. Nombre destino
    char dummy_dest[101];
    n = read(cliSocket, dummy_dest, len_dest);
    dummy_dest[n] = '\0';
    printf("2. Recibí nombre destino: '%s' (%d bytes)\n", dummy_dest, n);

    // 3. Longitud del nombre del archivo
    n = read(cliSocket, buffer, 100);
    buffer[n] = '\0';
    int len_nombre = atoi(buffer);
    printf("3. Recibí longitud nombre archivo: '%s' (%d bytes)\n", buffer, n);

    // 4. Nombre del archivo
    char nombre_archivo[101];
    n = read(cliSocket, nombre_archivo, len_nombre);
    nombre_archivo[n] = '\0';
    printf("4. Recibí nombre archivo: '%s' (%d bytes)\n", nombre_archivo, n);

    // 5. Tamaño del archivo
    char buffer_size[19];
    n = read(cliSocket, buffer_size, 18);
    buffer_size[n] = '\0';
    long file_size = atol(buffer_size);
    printf("5. Recibí tamaño archivo: '%s' (%d bytes)\n", buffer_size, n);

    // 6. Contenido del archivo
    printf("6. Esperando contenido del archivo (%ld bytes)...\n", file_size);
    
    char* contenido = (char*)malloc(file_size);
    long remaining = file_size;
    long offset1 = 0;

    while (remaining > 0) {
        int chunk_size = remaining > 300 ? 300 : remaining;
        char temp_buffer[300]; // buffer temporal

        int n = read(cliSocket, temp_buffer, chunk_size);
        printf("El número es: %d\n", n);  // n es int, no hace falta %ld

        if (n <= 0) {
            printf("Error en recepción: n=%d\n", n);
            break;
        }

        memcpy(contenido + offset1, temp_buffer, n);  // copiamos al buffer final
        offset1+= n;
        remaining -= n;
        }
    
    printf("   Contenido recibido correctamente (%ld bytes)\n", file_size);
    

    printf("\n--- PREPARANDO REENVÍO ---\n");
    // Construir mensaje nuevo (con origen minombre)
    int len_origen = minombre.size();
    int total_size = 5 + 1 + 5 + len_origen + 100 + len_nombre + 18 + file_size;
    printf("Tamaño total del mensaje: %d bytes\n", total_size);

    char* mensaje = (char*)malloc(total_size);
    int offset = 0;

    // Tamaño total
    sprintf(buffer, "%05d", total_size);
    memcpy(mensaje + offset, buffer, 5); offset += 5;
    printf("- Cabecera tamaño: '%s'\n", buffer);

    // Tipo de mensaje
    mensaje[offset++] = 'f';
    printf("- Tipo mensaje: 'f'\n");

    // Longitud del nombre del remitente
    sprintf(buffer, "%05d", len_origen);
    memcpy(mensaje + offset, buffer, 5); offset += 5;
    printf("- Longitud remitente: '%s' (%d)\n", buffer, len_origen);

    // Nombre del remitente (minombre)
    memcpy(mensaje + offset, minombre.c_str(), len_origen);
    offset += len_origen;
    printf("- Remitente: '%.*s'\n", len_origen, minombre.c_str());

    // Longitud del nombre del archivo
    sprintf(buffer, "%0100d", len_nombre);
    memcpy(mensaje + offset, buffer, 100); offset += 100;
    printf("- Longitud nombre archivo: '%s'\n", buffer);

    // Nombre del archivo
    memcpy(mensaje + offset, nombre_archivo, len_nombre);
    offset += len_nombre;
    printf("- Nombre archivo: '%s'\n", nombre_archivo);

    // Tamaño del archivo
    sprintf(buffer, "%018ld", file_size);
    memcpy(mensaje + offset, buffer, 18);
    offset += 18;
    printf("- Tamaño archivo: '%s'\n", buffer);

    // Contenido del archivo (no imprimimos el contenido binario)
    memcpy(mensaje + offset, contenido, file_size);
    offset += file_size;
    printf("- Contenido archivo: [%ld bytes de datos binarios]\n", file_size);

    // Reenviar al cliente destino si existe
    if (nickname.count(dummy_dest) > 0) {
        printf("\n--- REENVIANDO A %s ---\n", dummy_dest);
        write(nickname[dummy_dest], mensaje, total_size);
        printf("Archivo reenviado de %s a %s\n", minombre.c_str(), dummy_dest);
    } else {
        printf("\nERROR: Usuario destino no encontrado: %s\n", dummy_dest);
    }

    // Limpiar
    free(contenido);
    free(mensaje);
    printf("--- FIN DE PROCESAMIENTO DE ARCHIVO ---\n\n");
}


  } 
  


int main() {
    int SocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFD < 0) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(7200);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(SocketFD, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    if (listen(SocketFD, 10) < 0) {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    fd_set master_set, read_fds;
    FD_ZERO(&master_set);
    FD_SET(SocketFD, &master_set);
    int max_sd = SocketFD;
    map<int, string> nombres_por_socket;

    while (true) {
        read_fds = master_set;
        if (select(max_sd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select()");
            break;
        }

        for (int i = 0; i <= max_sd; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == SocketFD) {
                    int nuevoSocket = accept(SocketFD, NULL, NULL);
                    if (nuevoSocket < 0) {
                        perror("accept()");
                        continue;
                    }

                    char buffer[256];
                    int n = read(nuevoSocket, buffer, 5);
                    if (n <= 0) continue;
                    buffer[n] = '\0';
                    int tamano = atoi(buffer);
                    n = read(nuevoSocket, buffer, 1);
                    if (buffer[0] == 'N') {
                        n = read(nuevoSocket, buffer, tamano - 1);
                        buffer[n] = '\0';
                        string nombre(buffer);
                        nickname[nombre] = nuevoSocket;
                        nombres_por_socket[nuevoSocket] = nombre;
                        cout << "!!bienvenido!! --> " << nombre << endl;
                        FD_SET(nuevoSocket, &master_set);
                        if (nuevoSocket > max_sd) max_sd = nuevoSocket;
                    } else {
                        close(nuevoSocket);
                    }

                } else {


                  cout<<"si entre1"<< flush;
                    string nombre = nombres_por_socket[i];
                    bool desconectado = false;

                    char buffer[201];
                    int n = read(i, buffer, 5);
                    
                    if (n <= 0) {
                        
                        desconectado = true;
                    } else {
                     
                        buffer[n] = '\0';
                        int tamano = atoi(buffer);
                        n = read(i, buffer, 1);
                        if (n <= 0) {
                            desconectado = true;
                        } 
                      
                        
                        else {
                          
                            char tipo = buffer[0];

                            if(tipo=='Q'){
                             
                              desconectado = true;
                            } 
                            else {procesarMensaje(i, nombre, tipo);}
                        }
                    }

                    if (desconectado) {
                        cout << "Cliente desconectado: " << nombre << endl;
                        close(i);
                        FD_CLR(i, &master_set);
                        nickname.erase(nombre);
                        nombres_por_socket.erase(i);
                    }
                }
            }
        }
    }

    close(SocketFD);
    return 0;
}