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
constexpr int BUFFER_SIZE = 300;
string nick;

//clase 


#include <cstdint> 

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
long numero_read2(int sockt,int cont){
    char nume[1000];
    int n=read (sockt,nume,cont);
    nume[n]='\0';
    return atol(nume);
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
         else if (texto2[0]=='F'){
            int t1=numero_read(sock,2);
            string desde=texto_read(sock,t1);
            int t4=numero_read(sock,3);
            string nameF= texto_read(sock,t4);
            
            long t2= numero_read2(sock,10);
            string contenido=recv_File(sock,t2);

            string leer= int_String(t1,2)+desde+int_String(t4,3)+nameF+int_String2(t2,10);
            printf("\nREAD---%c%s\n",texto2[0],leer.c_str());

            FILE* f = fopen(string("["+desde+"]"+nameF).c_str(), "wb");
            if (!f) {
                perror("No se pudo crear el archivo");
                break;
            }
            fwrite(contenido.data(), 1, t2, f);
            fclose(f);

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

    string total1= string("f")+int_String(para1.size(),2)+para1+int_String(texto.size(),3)+texto+ int_String2(file_size,10);
    printf("\n WRITE---%s\n",total1.c_str());
    
    
    write(sock,total1.data(),total1.size());
    write(sock,file_data.data(),file_size);
    

          
}
std::string serializarSala(const Sala &sala) {
    std::string buffer;

    size_t baseSize = sizeof(Sala) - sizeof(Cocina*);
    buffer.resize(baseSize);
    std::memcpy(buffer.data(), &sala, baseSize);

    size_t offset = buffer.size();
    buffer.resize(offset + sizeof(Cocina));
    std::memcpy(buffer.data() + offset, sala.cocina, sizeof(Cocina));

    return buffer;
}
void writeObjecto(int sock){

    Sala sala;
    string sa=string("J");
    int n=write(sock,sa.c_str(),1);
    sala.silla = {1, "Madera", 1.2f, 5.0f};
    sala.mesa = {2, "Rectangular", 2.0f, 1.0f};
    sala.cocina = new Cocina{3, "Moderna", 4, 1};
    sala.numeroVentanas = 2;
    strcpy(sala.descripcion, "Sala con comedor y cocina integrada");


    std::string buffer = serializarSala(sala);

    // Enviar
    write(sock, buffer.data(), buffer.size());

    std::cout << "Sala enviada (" << buffer.size() << " bytes):\n";
    
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
      string para1;
      //printf("\n para : ");
      //getline(cin,para1);
    

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
        printf("\n5 Enviar Objeto");
        printf("\n6: Salir");
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
        else if(option==5){
          writeObjecto(sock_fd);

        }
        else if ( option==6){
          salir(sock_fd);
          break;
        }

        
        
    }
    shutdown(sock_fd, SHUT_RDWR);
    close(sock_fd);
    return 0;
}