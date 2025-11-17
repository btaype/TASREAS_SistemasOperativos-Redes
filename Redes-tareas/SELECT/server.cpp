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
#include <vector>
#include<map>

#include <mutex>
#include <cstdint>




using namespace std;
constexpr int BUFFER_SIZE = 300;

//game para ams juagdores
struct Ticactoe{
    string utils="OX";
    string game="_________";
    map<int,char>juagadores;
    bool tur=0;
    char turnos[2]={'O','X'};

    int canjuagdas=0;
    bool existe(int id){
        return this->juagadores.find(id) != this->juagadores.end();
    }
    void anadir(int sock){
        if (utils.size()>0){
        char ch=utils[0];
        utils.erase(0,1);
        juagadores[sock]=ch;   }
        
        
    }
     int getsock(char simbolo){
        for (auto &p : juagadores){
            if (p.second==simbolo){
                return p.first;  
            }
        }
        return -1; 
    }
    void enviarTurno(){
        int socke=getsock(turnos[tur]);
        if (socke!=-1){
            string enviar="V"+string(1,turnos[tur]);
            printf("\nWRITE--%s\n",enviar.c_str());
            fflush(stdout);
            write(socke,enviar.data(),2);
            //tur=!tur;
        }


    }
    void hacerjugada(int pos,int sock){
        if (pos < 1 || pos > 9) return;              
        if (game[pos-1]!= '_') return;                 
        if (juagadores[sock]==turnos[tur]) {        
            game[pos-1]=turnos[tur];                 
            tur =!tur;     
            canjuagdas++;                         
        }
    }
    
    char ganador(){
        int combos[8][3] = {
            {0,1,2}, {3,4,5}, {6,7,8}, 
            {0,3,6}, {1,4,7}, {2,5,8}, 
            {0,4,8}, {2,4,6}           
        };

        for (auto &c : combos){
            char a = game[c[0]];
            char b = game[c[1]];
            char c3 = game[c[2]];

            if (a != '_' && a == b && b == c3){
                return a; 
            }
        }

        return '_';
    }
     void Printgame(){
        string enviar= "v"+game;
        for (auto &p : juagadores) {
            
            write(p.first,enviar.data(),enviar.size());
            printf("\nWRITE--%s\n",enviar.c_str());
            fflush(stdout);
            }
     }
    bool empate(){
        return canjuagdas>=9;
    }
};
bool salida = 1;

vector<Ticactoe> salas;
map<string,int>nick;
map<int,string>nick2;
mutex salas_mutex;

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
void enviarMensaje(int destino, const string &name, const string &texto){
    string total = string("T") 
                 + int_String(name.size(), 2) 
                 + name 
                 + int_String(texto.size(), 3) 
                 + texto;

    printf("\nEnviando--%s\n",total.c_str());
    fflush(stdout);

    write(destino, total.c_str(), total.size());
}
bool buscarSalas(int sock){
    lock_guard<mutex> lock(salas_mutex); 
    for (size_t i = 0; i < salas.size(); i++){
        
        if (salas[i].existe(sock)){
            
            return 0; }

        else if (salas[i].utils.size() == 0){
            continue; 
        }
        else {
          
            salas[i].anadir(sock);
            salas[i].enviarTurno();
            return 1;
        }
    }

    Ticactoe salA;
    salA.anadir(sock);
    salas.push_back(salA);
    return 1;
}

bool buscarmisala(int sock,int pos){
    lock_guard<mutex> lock(salas_mutex); 
    for (size_t i = 0; i < salas.size(); i++){
        
        if (salas[i].existe(sock)){
            salas[i].hacerjugada(pos,sock);
            salas[i].Printgame();
            char gan=salas[i].ganador();
            if ( gan!='_'){
                string name="TIC TAC TOE";
                string smg= "GANADOR -> "+string(1,gan);
                for (auto &p : salas[i].juagadores) {
                    enviarMensaje(p.first, name,smg);
                }
                salas.erase(salas.begin()+i);
                
                
            }
            else if ( salas[i].empate()){
                string name="TIC TAC TOE";
                string smg= "Felicidades, Qedaro empate";
                for (auto &p : salas[i].juagadores) {
                    enviarMensaje(p.first, name,smg);
                }
                salas.erase(salas.begin()+i);

            }

            else {
                salas[i].enviarTurno();
            }

            return 1;
             
        }

        
    }
 
    return 0;
    
}




void  readd(int sock,string name, char primer,fd_set *master_set){
    char texto2[2];
    
    
    texto2[0]=primer;
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
            printf("\nREAD--=%s\n",destino.c_str());
            fflush(stdout);
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
            close(sock);
            FD_CLR(sock, master_set);
            return;
            
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
         else if(texto2[0]=='o'){
            int t1=numero_read(sock,2);
            string desde=texto_read(sock,t1);
            long t2= numero_read2(sock,10);
            string contenido=recv_File(sock,t2);
            string leer= int_String(t1,2)+desde+int_String2(t2,10);
            printf("\nREAD---%c%s\n",texto2[0],leer.c_str());

             auto it = nick.find(desde);
            if (it != nick.end()) {
                string totalHead=string("O")+int_String(name.size(),2)+name+int_String2(t2,10);
                printf("\nWRITE---%s\n",totalHead.c_str());

                write(nick[desde],totalHead.c_str(),totalHead.size());
                write(nick[desde],contenido.data(),contenido.size());
            }  

    }

        else if(texto2[0]=='P'){
            printf("\nREAD--%c\n",texto2[0]);
            fflush(stdout);
            if( buscarSalas(sock)){
                printf("\nencontaste Sala\n");
                fflush(stdout);
            }
            else{printf("\nNO encontaste Sala\n");}
                
        }
        else if (texto2[0]=='W'){

            int pos=numero_read(sock,1);
            printf("\nREAD--%c%01d\n",texto2[0],pos);
            fflush(stdout);
            if(buscarmisala(sock,pos)){
                printf("\nSI ESATS PARA EL MOVIENTO\n");
                fflush(stdout);
            }

        }


  
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

    if (listen(SocketFD, 10) < 0) {
        perror("listen"); close(SocketFD); return 1;
    }

    cout << "Servidor esperando en puerto 1100...\n";

   
    fd_set master_set, read_fds;
    FD_ZERO(&master_set);
    FD_SET(SocketFD, &master_set);

    int max_sd = SocketFD;

    while (true) {
        read_fds = master_set;  
        char texto2[2];
        if (select(max_sd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select()");
            break;
        }

        
        for (int fd = 0; fd <= max_sd; fd++) {

            if (!FD_ISSET(fd, &read_fds)) continue;

            
            if (fd == SocketFD) {

                int ConnectFD = accept(SocketFD, NULL, NULL);
                if (ConnectFD < 0) { perror("accept"); continue; }

                cout << " new conexion " << ConnectFD << endl;

                FD_SET(ConnectFD, &master_set);
                if (ConnectFD > max_sd) max_sd = ConnectFD;
                int n1 = read( ConnectFD, texto2, 1);
                if (texto2[0] == 'n') {
                    n1 = numero_read( ConnectFD, 2);
                    string name = texto_read( ConnectFD,n1);
                    auto it = nick.find(name);
                    if (it != nick.end()) {
                         printf("\n Cliente Nuevo NO Conectado: %s\n", name.c_str());
                         fflush(stdout);
                        Error_(ConnectFD, 1);
                        close(fd);
                        FD_CLR(fd, &master_set);

                    continue;
                    } else {
                        printf("\n Cliente Nuevo Conectado: %s\n", name.c_str());
                        fflush(stdout);
                        nick[name] =  ConnectFD;
                        nick2[ ConnectFD] = name;
                    }
                }
                continue;
            }

            
            
            int n1 = read(fd, texto2, 1);

            
            if (n1 <= 0) {
                cout << " Cliente desconectado" << fd << "\n";
                close(fd);
                FD_CLR(fd, &master_set);

                
                for (auto it = nick.begin(); it != nick.end(); ) {
                    if (it->second == fd)
                        it = nick.erase(it);
                    else
                        ++it;
                }
                continue;
            }

            
            if (texto2[0] != 'n') {
                readd( fd, nick2[fd],texto2[0],&master_set);
                
            }
        }
    }

    close(SocketFD);
    return 0;
}