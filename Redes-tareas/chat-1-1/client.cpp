#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cstdio>
#include <thread>
using namespace std;
bool salida=1;
string nick;
string int_String(int n,int tamano){

  string num=to_string(n);
  int n1= tamano - num.size();
  for (int i=n1;i>0;i--){
    num=string("0")+num;

  } 
  return num;
}
string int_String2(long n,int tamano){

  string num=to_string(n);
  int n1= tamano - num.size();
  for (int i=n1;i>0;i--){
    num=string("0")+num;

  } 
  return num;
}
int numero_read(int sockt,int cont){
    char nume[9];
    int n=read (sockt,nume,cont);
    nume[n]='\0';
    return atoi(nume);
}
string texto_read(int sockt, int cont){
    char text[256];
    int n=read (sockt,text,cont);
    text[n]='\0';
    return string(text);
}


// Threads --> Reads
void  readd(int sock){
    char texto2[2];
    while (1){
        int n=read(sock,texto2,1);
        if (n<=0){
            return;
        }
        if (texto2[0]=='M'){
          int tamano=numero_read(sock,2);
          string source=texto_read(sock,tamano);
          tamano= numero_read(sock,3);
          string texto=texto_read(sock,tamano);
          printf("\n[ %s ] = %s\n",source.c_str(),texto.c_str());
          fflush(stdout);

        }
        else if (texto2[0]=='T'){
           int tamano=numero_read(sock,2);
            string source=texto_read(sock,tamano);
            tamano= numero_read(sock,3);
            string texto=texto_read(sock,tamano);
            printf("\n[ %s ] = %s\n",source.c_str(),texto.c_str());
            fflush(stdout);
        }
         else if (texto2[0]=='L'){
           int tamano=numero_read(sock,2);
            printf("\n-----LISTA-------:\n");
            fflush(stdout);
            tamano--;
           for (;tamano>=0;tamano--){

                
                int tamano2= numero_read(sock,2);
                string texto=texto_read(sock,tamano2);
                printf(" [ %s ] \n",texto.c_str());
                fflush(stdout);
        }
        

        }

         else if (texto2[0]=='E'){
            int tamano=numero_read(sock,3);
            string source=texto_read(sock,tamano);  
            printf("\n[Error]: %s\n",source.c_str());
            fflush(stdout);

         }
    }
    shutdown(sock, SHUT_RDWR);
    close(sock);
}


void write_broadcast(int sock, string msg ){
    char send2[4];
    sprintf(send2,"%03d",(int)msg.size());
    string enviar=string("m")+string(send2)+msg;
    printf("write:  %s\n", enviar.c_str());
    int n=write(sock,enviar.c_str(),enviar.size());
    return;
}
void salir(int sock){
  string sa=string("x");
  int n=write(sock,sa.c_str(),1);
}


void msg_client(int sock, string msg ,string client ){
  
  string send2=string("t")+int_String(client.size(),2)+client+int_String(msg.size(),3)+msg;
  printf("%s\n",send2.c_str());
  write(sock,send2.c_str(),send2.size());


}

void list_client(int sock){
  string sa=string("l");
  int n=write(sock,sa.c_str(),1);
}
void enviarFIle(int sock){
          string texto;
          string para1;
          printf("\n para : ");
          getline(cin,para1);
          printf("\n file name : ");
          getline(cin,texto);
     FILE* file = fopen(texto.c_str(), "rb");
    if (!file) {
        perror("No se pudo abrir el archivo");
        return;
    }

    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    string file_data;
    file_data.resize(file_size);
    
    fread(file_data.data(), 1, file_size, file);
    fclose(file);

    string total1= string("f")+int_String(para1.size(),2)+para1+int_String(texto.size(),3)+texto+ int_String2(file_size,15);
    printf("\ncabezeraFlie---%s\n",total1.c_str());
    for (size_t i = 0; i < file_data.size(); i++) {
        putchar(file_data[i]);
    }
    
    write(sock,total1.data(),total1.size());
    write(sock,file_data.data(),file_size);
    

          
}

int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) { perror("socket"); return 1; }
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1100);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    if (connect(sock_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect"); close(sock_fd); return 1;
    }

    string texto;
    printf("Nickname: ");
    getline(cin, texto);

    char num[4];
    sprintf(num,"%02d",(int)texto.size());

    string send1=string("n")+string(num)+texto;

    printf("%s\n", send1.c_str());
    write(sock_fd,send1.c_str(),send1.size());
    
    thread p1(readd, sock_fd);
    p1.detach();
    int option=0;
    while(salida){
        printf("\nMENU");
        printf("\n1: Broadcast");
        printf("\n2: msg client");
        printf("\n3: List client");
        printf("\n4 Enviar Archivo");
        printf("\n5: Salir");
        printf("\nOPTION: ");
        cin>>option;
        cin.ignore();
        if (option==1){
          printf("\n Broadcast msg : ");
          getline(cin,texto);
          write_broadcast(sock_fd,texto);
        

      }
        else if (option==2){
          string para1;
          printf("\n para : ");
          getline(cin,para1);
          printf("\n msg : ");
          getline(cin,texto);
          msg_client(sock_fd, texto ,para1 );


        }

        else if (option==3){
          list_client(sock_fd);


        }
        else if(option==4){

          enviarFIle(sock_fd);

        }
        else if ( option==5){
          salir(sock_fd);
          break;
        }

        
        
    }
    shutdown(sock_fd, SHUT_RDWR);
    close(sock_fd);
    return 0;
}