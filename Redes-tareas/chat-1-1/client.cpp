#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>

#include <thread>
using namespace std;
bool salida=1;
string nick;

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
void  readd(int sock){
    char texto2[2];
    while (1){
        int n=read(sock,texto2,1);
        if (n<=0){
            return;
        }
        if (texto2[0]=='n'){
            int tamano=numero_read(sock,2);
            string texto3= texto_read(sock,tamano);
            printf("Nickname Entrante: %s\n", texto3.c_str());
            nick=texto3;
        }
        if (texto2[0]=='m'){
            int tamano=numero_read(sock,3);
            string texto3= texto_read(sock,tamano);
            printf("<%s> dice: %s\n", nick.c_str(), texto3.c_str());
            if (texto3.compare("chau") == 0){
                salida=0;
                printf("cerrando thread\n");
                break;
            }
        }
    }
    shutdown(sock, SHUT_RDWR);
    close(sock);
}
bool write_msg(int sock, string msg ){
    char send2[4];
    sprintf(send2,"%03d",(int)msg.size());
    string enviar=string("m")+string(send2)+msg;
    printf("write:  %s\n", enviar.c_str());
    int n=write(sock,enviar.c_str(),enviar.size());
    if(n<0){
        return 0;
    }
    return 1;
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
    while(salida){
        printf("\nmsg: ");
        getline(cin, texto);
        if (texto.compare("chau") == 0){
            break;
        }
        bool r=write_msg(sock_fd, texto );
        if(!r){
            break;
        }
    }
    shutdown(sock_fd, SHUT_RDWR);
    close(sock_fd);
    return 0;
}
