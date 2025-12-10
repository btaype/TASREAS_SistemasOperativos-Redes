#define _FILE_OFFSET_BITS 64
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cstdio>
#include <thread>
#include <arpa/inet.h>  
#include <sys/types.h>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <mutex>
#include <fcntl.h>   

#include <sys/stat.h>   

#include <sys/mman.h>   


using namespace std;

namespace fs = std::filesystem;
constexpr int BUFFER_SIZE = 300;

off_t numero_read3(int sockt,int cont) {
    char nume[1000];
    int n=read(sockt,nume,cont);

    if (n <= 0){ return 0;}

    nume[n] = '\0';

    return (off_t) strtoll(nume, NULL, 10);
}
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
  int n1=tamano-num.size();
  for (int i=n1;i>0;i--){
    num=string("0")+num;

  } 
  return num;
}

string int_String3(off_t n,int tamano){

  string num=to_string((long long)n);
  int n1= tamano-num.size();
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
    char nume[100];
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


long numero_read2(int sockt,int cont){
    char nume[1000];
    int n=read (sockt,nume,cont);
    nume[n]='\0';
    return atol(nume);
}

void enviarFIle(int sock,string name,long x, long y){
         
     FILE* file = fopen(name.c_str(), "rb");
    if (!file) {
        return;
    }

    
    fseek(file, 0, SEEK_END);
    off_t file_size;
    file_size = ftello(file);
    rewind(file);

 

    string total1= string("f")+int_String2(x,10)+int_String2(y,10)+int_String(name.size(),10)+ name +  int_String3(file_size,20);


    printf("\n WRITE---%s\n",total1.c_str());
    fflush(stdout);
    
    write(sock,total1.data(),total1.size());


    const size_t tamanoEnvio=1024*1024; 
    char buffer5[tamanoEnvio];
    size_t read3;

    while (1) {
        read3 = fread(buffer5,1,tamanoEnvio, file);
        if (read3<=0){
            break;
        } 
        size_t total_envio = 0;

        while (total_envio<read3) {
            ssize_t sent = write(sock,buffer5+total_envio,read3-total_envio);
            if (sent<0) {
                return;
            }
            total_envio=total_envio+sent;
        }     
}
        
    fclose(file);
}
void recv_File(int sock,off_t tamano,string nombre_archivo,string carpeta) {
    bool exitearch=!fs::exists(carpeta);

    if ( exitearch== false) {
        fs::create_directories(carpeta);
    }
     nombre_archivo = carpeta + "/"+nombre_archivo;
    FILE* salida=fopen(nombre_archivo.c_str(),"wb");
    if (!salida) {
        
        return;
    }

    size_t sizeT = 1024 * 1024; 

    char* buffer = new char[sizeT];  

    off_t restantes=tamano;

    while(restantes>0){
        size_t aLeer;

        if (restantes>sizeT) {
            aLeer=sizeT;
        } else {
            aLeer=restantes;
        }
        ssize_t n=read(sock,buffer,aLeer);

        if (n<=0) {
           
            break;
        }

        fwrite(buffer,1,n,salida);
        restantes-=n;
    }

    fclose(salida);
}
string comletreicivirfile(int sock,long &filas, long &columnas, int worker,string carpAlcamacena){


    char texto2[2];
    
    int n=read(sock,texto2,1);
    if (texto2[0]=='f'){
        cout<<"reeecifile\n";
    string nameF;

    long x=numero_read2(sock,10);
            long filas=x;
            long y=numero_read2(sock,10);
            long columnas=y;
            int t4= numero_read(sock,10);

            nameF= texto_read(sock,t4);
            
            off_t t2= numero_read3(sock,20);
            
            size_t pos=nameF.rfind(".bin");
            string nuevoNombre;
            nuevoNombre=nameF.substr(0,pos)+to_string(worker)+nameF.substr(pos);
            
            recv_File(sock,t2,nuevoNombre,carpAlcamacena);
            
           
            string namefinall=carpAlcamacena+"/"+nuevoNombre;
            return namefinall;
            
            cout<<"reeecifile2\n";

    }

    return "";

}
void bin_a_csv(string namebin, string namec,long filas,long columnas){
    ifstream bin(namebin, ios::binary);
    ofstream csv(namec);

    double num; 
    for(long f=0;f<filas; f++){
        for(long c=0; c<columnas; c++){

            if(!bin.read((char*)&num,sizeof(double))) break;
            csv << num;

            if(c<columnas-1) csv << ",";
        }
        csv << "\n";
    }

    bin.close();
    csv.close();


}
void readd(int sock){
    string carpAlcamacena="svd";
    char texto2[2];
    while(1){
    int n=read(sock,texto2,1);
    if (texto2[0]=='f'){
        cout<<"reeecifile\n";
    string nameF;

    long x=numero_read2(sock,10);
            long filas=x;
            long y=numero_read2(sock,10);
            long columnas=y;
            int t4= numero_read(sock,10);

            nameF= texto_read(sock,t4);
            
            off_t t2= numero_read3(sock,20);
            
            size_t pos=nameF.rfind(".bin");
            string nuevoNombre;
            nuevoNombre=nameF.substr(0,pos)+to_string(2)+nameF.substr(pos);
            
            recv_File(sock,t2,nuevoNombre,carpAlcamacena);
            
            //transpuestas[sockt][i]=carpeta+"/"+nuevoNombre;
            string namefinall=carpAlcamacena+"/"+nuevoNombre;
             bin_a_csv(namefinall ,carpAlcamacena+ "/"+nuevoNombre+".csv",x,y);
            
            
    }  





}
}

int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) { perror("socket"); return 1; }
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    if (connect(sock_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect"); close(sock_fd); return 1;
    }
    long x=1000;
    long y=1000;
     long filas_temp = x;
    long cols_temp = y;
    
    string nombreach= "matrix.csv";
    string SVD="svd";
    enviarFIle(sock_fd,nombreach,x, y);
    thread t(readd, sock_fd);
    t.detach();
    while(1){


    }


   



   
}