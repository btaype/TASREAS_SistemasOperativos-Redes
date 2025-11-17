#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cstdio>
#include <thread>
#include <arpa/inet.h>  
#include <cmath> 
#include <map>
#include <netdb.h>
using namespace std;
bool salida=1;
constexpr int BUFFER_SIZE = 300;
#define PORT 1100
#define NPAKETE 777
#define NHEAD 5
string nick;
char sym;
string tab="_________";
//clase 


#include <cstdint> 

#pragma pack(push, 1) 

struct Sala {
    int id;  
    int numeroVentanas;
    char descripcion[1000]; 
    string nombre;
    float largo;
    float ancho;

  static uint32_t floatToNetwork(float f) {
        uint32_t temp;
        std::memcpy(&temp, &f, sizeof(float));
        return htonl(temp);
    }

    static float networkToFloat(uint32_t net) {
        uint32_t temp = ntohl(net);
        float f;
        std::memcpy(&f, &temp, sizeof(float));
        return f;
    }
  std::string serializarSala(const Sala &s) {
    std::string buffer;

  

    
        uint32_t id_net = htonl(id);
        buffer.append(reinterpret_cast<const char*>(&id_net), 4);

        
        uint32_t ventanas_net = htonl(numeroVentanas);
        buffer.append(reinterpret_cast<const char*>(&ventanas_net), 4);

        
        uint32_t len_nombre = htonl(nombre.size());
        buffer.append(reinterpret_cast<const char*>(&len_nombre), 4);
        buffer.append(nombre);

        
        uint32_t len_desc = htonl(std::strlen(descripcion));
        buffer.append(reinterpret_cast<const char*>(&len_desc), 4);
        buffer.append(descripcion, std::strlen(descripcion));

        
        int parteEnteraL = static_cast<int>(largo);
        int parteDecimalL = static_cast<int>((largo - parteEnteraL) * 1000000);

        cout<< parteEnteraL<<" ; " <<parteDecimalL<<endl;

        uint32_t enteroL_net = htonl(parteEnteraL);
        uint32_t decimalL_net = htonl(parteDecimalL);
        buffer.append(reinterpret_cast<const char*>(&enteroL_net), 4);
        buffer.append(reinterpret_cast<const char*>(&decimalL_net), 4);

       

        int parteEnteraA = static_cast<int>(ancho);
        int parteDecimalA = static_cast<int>((ancho - parteEnteraA) * 1000000);

        uint32_t enteroA_net = htonl(parteEnteraA);
        uint32_t decimalA_net = htonl(parteDecimalA);
        buffer.append(reinterpret_cast<const char*>(&enteroA_net), 4);
        buffer.append(reinterpret_cast<const char*>(&decimalA_net), 4);


        return buffer;

  }
static  Sala deserializarSala(const std::string &buffer) {
    Sala s;
        size_t offset = 0;

      
        uint32_t id_net;
        std::memcpy(&id_net, buffer.data() + offset, 4);
        s.id = ntohl(id_net);
        offset += 4;

        
        uint32_t ventanas_net;
        std::memcpy(&ventanas_net, buffer.data() + offset, 4);
        s.numeroVentanas = ntohl(ventanas_net);
        offset += 4;

       
        uint32_t len_nombre_net;
        std::memcpy(&len_nombre_net, buffer.data() + offset, 4);
        uint32_t len_nombre = ntohl(len_nombre_net);
        offset += 4;

        s.nombre.assign(buffer.data() + offset, len_nombre);
        offset += len_nombre;

       
        uint32_t len_desc_net;
        std::memcpy(&len_desc_net, buffer.data() + offset, 4);
        uint32_t len_desc = ntohl(len_desc_net);
        offset += 4;

        std::memcpy(s.descripcion, buffer.data() + offset, len_desc);
        s.descripcion[len_desc] = '\0'; 

        offset +=len_desc;

        uint32_t enteroL_net, decimalL_net;
        std::memcpy(&enteroL_net, buffer.data() + offset, 4);
        offset += 4;
        std::memcpy(&decimalL_net, buffer.data() + offset, 4);
        offset += 4;

       
        int parteEnteraL = ntohl(enteroL_net);
        int parteDecimalL = ntohl(decimalL_net);
        s.largo = parteEnteraL + (parteDecimalL / 1000000.0f);

        
        uint32_t enteroA_net, decimalA_net;
        std::memcpy(&enteroA_net, buffer.data() + offset, 4);
        offset += 4;
        std::memcpy(&decimalA_net, buffer.data() + offset, 4);
        offset += 4;
        int parteEnteraA = ntohl(enteroA_net);
        int parteDecimalA = ntohl(decimalA_net);
        s.ancho = parteEnteraA + (parteDecimalA / 1000000.0f);
        return s;
}
};

#pragma pack(pop)




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

void michis(string &paketeG,const int &nbody){
  int m=abs(static_cast<int>(nbody-paketeG.size()));
  for (int i=0;i<m;i++){
    paketeG=paketeG+"#";
  }


}

map<int,string> Dividies(string &paketeG){
  if(paketeG.size()<=0){
    return {};
  }
  else {
    map<int,string> DV;
    int numbody=NPAKETE-NHEAD;
    int num=paketeG.size()/(numbody);
    for(int i=0;i<=num-1;i++){
      string subtotal=int_String(i+1,NHEAD)+paketeG.substr(0,numbody);

      paketeG.erase(0,numbody);
      DV[i+1]=subtotal;
      

    }
    michis(paketeG,numbody);
    DV[0]=paketeG;

    return DV;






  }

}
void EnviarUDP(string &paketeG,int sock,const struct sockaddr_in &dest){
  map<int,string> listpakete=Dividies(paketeG);
  if (listpakete.size()<=0){
    return;
  }
  for (int i=1;i<listpakete.size();i++){
    sendto(sock,listpakete[i].c_str(),listpakete[i].size(),0,reinterpret_cast<const struct sockaddr*>(&dest),static_cast<socklen_t>(sizeof(dest)));

  }
  sleep(1);
  sendto(sock,listpakete[0].c_str(),listpakete[0].size(),0,reinterpret_cast<const struct sockaddr*>(&dest),static_cast<socklen_t>(sizeof(dest)));

}


int numero_read(string &data,int cont) {
    string parte=data.substr(0, cont); 
    int numero=stoi(parte);
    data.erase(0, cont);
    return numero;
}
string texto_read(string &data,int cont){
    string parte=data.substr(0,cont);
    data.erase(0,cont);
    return parte;
}


string recv_File(string &src,long tamano){
    string parte=src.substr(0,tamano);
    src.erase(0,tamano);
    return parte;
}

long numero_read2(string &data,int cont){
    string parte=data.substr(0,cont);
    data.erase(0,cont);
    return stol(parte);
}
// Threads --> Reads

void printtab(string tabl){
    int u=0;
    cout<<endl;
    for (int i=0 ; i<3;i++){
        cout<<" | ";
        for (int j=0 ; j<3;j++){
            cout<< tab[u];
            cout<<" | " ;
            u++;
        }

        cout<<endl;
        cout<<" ----------------------- \n";


    }


}
void readd2(map<int,string>listpakete,int sock,struct sockaddr_in &src){
   string total;
    for(auto &p:listpakete){
        if(p.first==0)continue;
        total+=p.second;
    }
    string fin=listpakete.at(0);
    fin.erase(remove(fin.begin(),fin.end(),'#'),fin.end());
    total+=fin;
    
    //---------------------------------------------Procesode lectura del string junatdo
        string texto2=texto_read(total,1);
        
        if (texto2=="M"){
          int tamano=numero_read(total,2);
          string source=texto_read(total,tamano);
          tamano= numero_read(total,3);
          string texto=texto_read(total,tamano);
          printf("\n[ %s ] = %s\n",source.c_str(),texto.c_str());
          fflush(stdout);

        }
        else if (texto2=="T"){
           int tamano=numero_read(total,2);
            string source=texto_read(total,tamano);
            tamano= numero_read(total,3);
            string texto=texto_read(total,tamano);
            printf("\n[ %s ] = %s\n",source.c_str(),texto.c_str());
            fflush(stdout);
        }
         else if (texto2=="L"){
           int tamano=numero_read(total,2);
            printf("\n-----LISTA-------:\n");
            fflush(stdout);
            tamano--;
           for (;tamano>=0;tamano--){

                
                int tamano2= numero_read(total,2);
                string texto=texto_read(total,tamano2);
                printf(" [ %s ] \n",texto.c_str());
                fflush(stdout);
        }
        

        }

         else if (texto2=="E"){
            int tamano=numero_read(total,3);
            string source=texto_read(total,tamano);  
            printf("\n[Error]: %s\n",source.c_str());
            fflush(stdout);

         }
         else if (texto2=="F"){
            int t1=numero_read(total,2);
            string desde=texto_read(total,t1);
            int t4=numero_read(total,3);
            string nameF= texto_read(total,t4);
            
            long t2= numero_read2(total,10);
            string contenido=recv_File(total,t2);

            string leer= int_String(t1,2)+desde+int_String(t4,3)+nameF+int_String2(t2,10);
            printf("\nREAD---%c%s\n",texto2[0],leer.c_str());

            FILE* f = fopen(string("["+desde+"]"+nameF).c_str(), "wb");
            if (!f) {
                perror("No se pudo crear el archivo");
                return;
            }
            fwrite(contenido.data(), 1, t2, f);
            fclose(f);

         }
        else if (texto2=="O"){
            int t1=numero_read(total,2);
            string desde=texto_read(total,t1);
            
            
            
            long t2= numero_read2(total,10);
            string contenido=recv_File(total,t2);

            string leer= int_String(t1,2)+desde+int_String2(t2,10);
            printf("\nREAD---%c%s\n",texto2[0],leer.c_str());
            Sala S1=Sala::deserializarSala(contenido);

          
            std::cout << "ID: " << S1.id << "\n";
            std::cout << "Ventanas: " << S1.numeroVentanas << "\n";
            std::cout << "Nombre: " << S1.nombre << "\n";
            std::cout << "Descripcion: " << S1.descripcion << "\n";
            std::cout << "Largo: " << S1.largo << "\n";
            std::cout << "Ancho: " << S1.ancho << "\n";

           

         }
         else if (texto2=="V"){

            string rr=texto_read(total,1);
            sym=rr[0];
            printf("\nREAD--%s%s\n",texto2.c_str(),rr.c_str());
            fflush(stdout);
            printf("\n<TIC TAC TOE>TU turno de Enviar tu JUgada %c\n",rr[0]);
            fflush(stdout);
         }
         else if (texto2=="v"){

            char rr[3];

            string game=texto_read(total,9);
            tab = game;
            printf("\nREAD--%s%s\n",texto2.c_str(),game.c_str());
            fflush(stdout);
            printtab(tab);

        

            }
    



}
void readUDP(int sock,struct sockaddr_in &src){
  map<int,string> listpakete;
  while(1){

    char buffer[NPAKETE+1];
    socklen_t addr_len = sizeof(src);

    ssize_t n = recvfrom(sock,buffer,sizeof(buffer)-1, 0,(struct sockaddr*)&src, &addr_len);

    if (n < 0) {
          perror("SALIENDO");
          return ;
    }

    buffer[n] = '\0'; 
    string paketeE=buffer;
    printf("<\nREAD>--->%s\n",paketeE.c_str());
    fflush(stdout);

    int numhead=numero_read(paketeE,NHEAD);
    listpakete[numhead]=paketeE;
    if (numhead==0){
      thread t(readd2,move(listpakete), sock, ref(src));
      t.detach();
      listpakete.clear();
    }

  }

}




void write_broadcast(int sock,string msg,const struct sockaddr_in &dest){
    char send2[4];
    sprintf(send2,"%03d",(int)msg.size());
    string enviar=string("m")+string(send2)+msg;
    printf("write:  %s\n", enviar.c_str());
    EnviarUDP(enviar,sock,dest);
    return;
}
void salir(int sock,const struct sockaddr_in &dest){
  string sa=string("x");
  EnviarUDP(sa,sock,dest);
}


void msg_client(int sock, string msg ,string client,const struct sockaddr_in &dest){
  
  string send2=string("t")+int_String(client.size(),2)+client+int_String(msg.size(),3)+msg;
  printf("%s\n",send2.c_str());
  EnviarUDP(send2,sock,dest);


}

void list_client(int sock,const struct sockaddr_in &dest){
  string sa=string("l");
  EnviarUDP(sa,sock,dest);
}
void enviarFIle(int sock,const struct sockaddr_in &dest){
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

    string total1= string("f")+int_String(para1.size(),2)+para1+int_String(texto.size(),3)+texto+ int_String2(file_size,10);
    printf("\n WRITE---%s\n",total1.c_str());
    fflush(stdout);
    string enviar= total1+file_data;
    EnviarUDP(enviar,sock,dest);
    

          
}

void writeObjecto(int sock){

  Sala sala1;
  sala1.id = 42;
  sala1.numeroVentanas = 3;
  sala1.nombre = "Sala de Conferencias";
  std::strncpy(sala1.descripcion, "Espacio amplio con proyector y sillas.", sizeof(sala1.descripcion));
  sala1.largo = 12.5f;
  sala1.ancho = 8.75f;

  std::cout << "ID: " << sala1.id << "\n";
  std::cout << "Ventanas: " << sala1.numeroVentanas << "\n";
  std::cout << "Nombre: " << sala1.nombre << "\n";
  std::cout << "Descripcion: " << sala1.descripcion << "\n";
  std::cout << "Largo: " << sala1.largo << "\n";
  std::cout << "Ancho: " << sala1.ancho << "\n";
  

  std::string data = sala1.serializarSala(sala1);
  string texto;
  string para1;
  printf("\n para : ");
  getline(cin,para1);
  string total1= string("o")+int_String(para1.size(),2)+para1+int_String(data.size(),10);
  write(sock,total1.data(),total1.size()) ;
  write(sock,data.data(),data.size());

  
}

void enviapos(int sock,const struct sockaddr_in &dest){
    
    int pos;
    printtab(tab);
    while(true)
    {   
        cout<<"\npos: ";
        cin>>pos;

        if (!(pos>=1 && pos<=9)){
            continue;

        }
        if( sym=='W' ){
            break;

        }
        if( tab[pos-1]== '_' ){
            string total = "W"+to_string(pos)+ sym;
            printf("\nWRITE--%s\n",total.c_str());
            fflush(stdout);
            EnviarUDP(total,sock,dest);
            sym='W';
            return;

        }
       
        
    }
    

   printf("\nNo es tu Turno\n");

} 

void enviarP(int sock,const struct sockaddr_in &dest){
    string total = "P";
    printf("\nWRITE--%s\n",total.c_str());
    fflush(stdout);
    EnviarUDP(total,sock,dest);
    tab="_________";


}
int main() {
    int sock_fd;
    
    std::string hello = "Hello from client";
    struct sockaddr_in servaddr{};
    
   
    struct hostent* host = gethostbyname("127.0.0.1");
    if (!host) {
        std::cerr << "Error obteniendo host\n";
        exit(EXIT_FAILURE);
    }

    
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

   
    memset(&servaddr, 0, sizeof(servaddr));

    
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr = *((struct in_addr*)host->h_addr);

    string texto;
    printf("Nickname: ");
    getline(cin, texto);

    char num[4];
    sprintf(num,"%02d",(int)texto.size());

    string send1=string("n")+string(num)+texto;

    printf("%s\n", send1.c_str());
    EnviarUDP(send1,sock_fd,servaddr);
    
    thread p1(readUDP, sock_fd,ref(servaddr));
    p1.detach();
    int option=0;
    while(salida){
        printf("\nMENU");
        printf("\n1: Broadcast");
        printf("\n2: msg client");
        printf("\n3: List client");
        printf("\n4 Enviar Archivo");
        printf("\n5 Enviar Objeto");
        
        printf("\n6: Conectar");
        printf("\n7: Enviar pos");
        printf("\n8: Salir");
        printf("\nOPTION: ");
        
        cin>>option;
        cin.ignore();
        if (option==1){
          printf("\n Broadcast msg : ");
          getline(cin,texto);
          write_broadcast(sock_fd,texto,servaddr);
          

      }
        else if (option==2){
          string para1;
          printf("\n para : ");
          getline(cin,para1);
          printf("\n msg : ");
          getline(cin,texto);
          msg_client(sock_fd, texto ,para1,servaddr);


        }

        else if (option==3){
          list_client(sock_fd,servaddr);


        }
        else if(option==4){

          enviarFIle(sock_fd,servaddr);

        }
       
        else if ( option==8){
          salir(sock_fd,servaddr);
          break;
        }
        else if ( option==6){
            enviarP(sock_fd,servaddr);
          
          }
          else if ( option==7){
            enviapos(sock_fd,servaddr);
          
          }
        
        
    }
    
    close(sock_fd);
    return 0;
}