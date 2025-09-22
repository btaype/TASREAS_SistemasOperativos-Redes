#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <thread>
#include <cstdio>
#include <cstdlib>

#include<map>
using namespace std;
constexpr int BUFFER_SIZE = 300;

bool salida = 1;
map<string,int>nick;


#include <iostream>
#include <cstring>
#include <cstdint>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>

#pragma pack(push, 1)

struct Silla {
    int32_t id;
    char material[50];
    float altura;
    float peso;
};

struct Mesa {
    int32_t id;
    char tipo[50];
    float largo;
    float ancho;
};

struct Cocina {
    int32_t id;
    char estilo[50];
    int32_t numHornillas;
    uint8_t tieneHorno;
};

struct Sala {
    Silla silla;
    Mesa mesa;
    Cocina *cocina;
    int32_t numeroVentanas;
    char descripcion[1000];
};



Sala deserializarSala(const std::string &buffer) {
    Sala sala{};
    size_t baseSize = sizeof(Sala) - sizeof(Cocina*);

    std::memcpy(&sala, buffer.data(), baseSize);

    sala.cocina = new Cocina;
    std::memcpy(sala.cocina, buffer.data() + baseSize, sizeof(Cocina));

    return sala;
}

string int_String(int n,int tamano){

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
long numero_read2(int sockt,int cont){
    char nume[1000];
    int n=read (sockt,nume,cont);
    nume[n]='\0';
    return atol(nume);
}



string read_textLong(int sockt,int t){
    char text[BUFFER_SIZE];
    int t2=BUFFER_SIZE;

    if (t<BUFFER_SIZE){
        t2=t;
    }
    string total;
    total.reserve(t2);
    int sum=0;
    
    while(t2){
        int n=read (sockt,text,t2);
        
        
        total.append(text,(size_t)n);
        
        t2=t2-n;
    }

    
    
    return  total;
}
string int_String2(long n,int tamano){

  string num=to_string(n);
  int n1= tamano - num.size();
  for (int i=n1;i>0;i--){
    num=string("0")+num;

  } 
  return num;
}

string recv_File(int sock, long tamano){
    string total;
    total.reserve(tamano); 
    int r=0;
    while(tamano){
        if(tamano-BUFFER_SIZE>=0){
        int dd=BUFFER_SIZE;
        string sub = read_textLong(sock,BUFFER_SIZE);
        fflush(stdout);

         total.append(sub.data(), sub.size());
        tamano=tamano-BUFFER_SIZE;
        r++;
        
        }
        else {
            
            
           string sub=read_textLong(sock,static_cast<int>(tamano));  
           total.append(sub.data(), sub.size());
            r++;
            break;
            
        }
        

    }
    printf("\nnum->%d",r);
    fflush(stdout);
    return total;

}
string texto_read(int sockt, int cont){
    char text[256];
    int n=read (sockt,text,cont);
    text[n]='\0';
    return string(text);
}


void Error_(int sock,int error){
    string err="Error no identificado";
    if (error==1){
        err="NickName ya existe";

    }
    else if (error==2){
        err="No existe NickName";
    }
    string total=string("E")+int_String(err.size(),3)+err;
    write(sock,total.c_str(),total.size());

}



void  readd(int sock,string name){
    char texto2[2];
    while (1){
        int n=read(sock,texto2,1);
        if (n<=0){
            
            break;
        }
        if (texto2[0]=='m'){
            int tamano=numero_read(sock,3);
            string texto=texto_read(sock,tamano);
            
            string total=string("M")+int_String(name.size(),2)+name+int_String(tamano,3)+texto;
            printf("\nEnviando--[%s]\n=%s",name.c_str(),total.c_str());
            for (auto &par : nick) {
                
                write(par.second ,total.c_str(),total.size());
                
            }

            

        }
        else if (texto2[0]=='t'){

            int tamano=numero_read(sock,2);
            string destino=texto_read(sock,tamano);
            tamano= numero_read(sock,3);
            string texto=texto_read(sock,tamano);

            auto it = nick.find(destino);

            if (it != nick.end()) {
                string total=string("T")+int_String(name.size(),2)+name+int_String(tamano,3)+texto;
                printf("\nEnviando--[%s]=%s\n",name.c_str(),total.c_str());
                fflush(stdout);
                write(nick[destino],total.c_str(),total.size());

            } else {
                
               Error_(sock,2);
            }
           

        }
         else if (texto2[0]=='l'){
            
            fflush(stdout);
            string total=string("L")+int_String(nick.size()-1,2);
            for (auto &par : nick) {
                if (par.first!= name) {
                total=total+int_String(par.first.size(),2)+par.first;
                
                }
                
            }
            printf("\nEnviando--[%s]=%s\n",name.c_str(),total.c_str());
            fflush(stdout);
            write(sock,total.c_str(),total.size());

        

        }

         else if (texto2[0]=='x'){
            printf("\n SALIENDO -- %s\n",name.c_str());
            nick.erase(name);
            break;
            
         }
          else if (texto2[0]=='f'){
            int t1=numero_read(sock,2);
            string desde=texto_read(sock,t1);
            int t4=numero_read(sock,3);
            string nameF= texto_read(sock,t4);
            
            long t2= numero_read2(sock,10);
            string contenido=recv_File(sock,t2);

            string leer= int_String(t1,2)+desde+int_String(t4,3)+nameF+int_String2(t2,10);
            printf("\nREAD---%c%s\n",texto2[0],leer.c_str());
            fflush(stdout);

            
             auto it = nick.find(desde);

            if (it != nick.end()) {
                string totalHead=string("F")+int_String(name.size(),2)+name+int_String(t4,3)+nameF+int_String2(t2,10);
                printf("\nWRITE---%s\n",totalHead.c_str());

                write(nick[desde],totalHead.c_str(),totalHead.size());
                write(nick[desde],contenido.data(),contenido.size());
            }
            /* 
            FILE* f = fopen(string(desde+nameF).c_str(), "wb");
            if (!f) {
                perror("No se pudo crear el archivo");
                break;
            }
            fwrite(contenido.data(), 1, t2, f);
            fclose(f);
            */

            
         }
         else if(texto2[0]=='J'){

            std::string buffer;
            buffer.resize(sizeof(Sala) - sizeof(Cocina*) + sizeof(Cocina));
            read(sock, buffer.data(), buffer.size());
            Sala sala = deserializarSala(buffer);

            std::cout << "Sala recibida:\n";
    std::cout << "  Silla: id=" << sala.silla.id 
              << ", material=" << sala.silla.material 
              << ", altura=" << sala.silla.altura 
              << ", peso=" << sala.silla.peso << "\n";

    std::cout << "  Mesa: id=" << sala.mesa.id 
              << ", tipo=" << sala.mesa.tipo 
              << ", largo=" << sala.mesa.largo 
              << ", ancho=" << sala.mesa.ancho << "\n";

    std::cout << "  Cocina: id=" << sala.cocina->id 
              << ", estilo=" << sala.cocina->estilo 
              << ", hornillas=" << sala.cocina->numHornillas 
              << ", horno=" << (sala.cocina->tieneHorno ? "Sí" : "No") << "\n";

    std::cout << "  Ventanas: " << sala.numeroVentanas << "\n";
    std::cout << "  Descripción: " << sala.descripcion << "\n";
         }
    }
    shutdown(sock, SHUT_RDWR);
    close(sock);
    printf("\n[%s]-> SALIENDO\n",name.c_str());
    nick.erase(name);
    fflush(stdout);
}

int main() {
    int SocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFD < 0) { perror("socket"); return 1; }

    int yes = 1;
    setsockopt(SocketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in stSockAddr{};
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port   = htons(1100);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(SocketFD, (sockaddr*)&stSockAddr, sizeof(stSockAddr)) < 0) {
        perror("bind"); close(SocketFD); return 1;
    }
    if (listen(SocketFD, 1) < 0) {
        perror("listen"); close(SocketFD); return 1;
    }

    cout << "Servidor esperando en puerto 1100...\n";

    
    while (true) {
        int ConnectFD = accept(SocketFD, nullptr, nullptr);
        if (ConnectFD < 0) { perror("accept"); continue; }
        
        char texto2[2];

        int n1=read(ConnectFD,texto2,1);
        if (texto2[0]=='n'){
            n1= numero_read(ConnectFD,2);
            string name=texto_read(ConnectFD,n1);
     

            auto it = nick.find(name);

            if (it != nick.end()) {
                
                Error_(ConnectFD,1);
                continue;
            } else {
                printf("\n Cliente : %s\n",name.c_str());
                nick[name]=ConnectFD;
                thread p1(readd, ConnectFD,name);
                p1.detach();
            }

        }

        
        
        
        
        
        
        
    }

    close(SocketFD);
    return 0;
}