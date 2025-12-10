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
#include <sys/types.h>
#include <mutex>
#include <cstdint>

#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>


#include <fstream>
#include <sstream>
#include <filesystem>
using namespace std;

namespace fs = std::filesystem;


using namespace std;
constexpr int BUFFER_SIZE = 300;

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
int numero_read(int sockt,int cont){
    char nume[100];
    int n=read (sockt,nume,cont);
    nume[n]='\0';
    return atoi(nume);
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


void recv_File(int sock,off_t tamano,string nombre_archivo,string carpeta) {
    if (!fs::exists(carpeta)) {
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
long numero_read2(int sockt,int cont){
    char nume[1000];
    int n=read (sockt,nume,cont);
    if (n <= 0){ return 0;}
    nume[n]='\0';
    return atol(nume);
}

off_t numero_read3(int sockt,int cont) {
    char nume[1000];
    int n=read(sockt,nume,cont);

    if (n <= 0){ return 0;}

    nume[n] = '\0';

    return (off_t) strtoll(nume, NULL, 10);
}

string texto_read(int sockt, int cont){
    char text[256];
    int n=read (sockt,text,cont);
    text[n]='\0';
    return string(text);
}



void enviarFIle(int sock,string name,string name2,long x, long y){
         
     FILE* file = fopen(name.c_str(), "rb");
    if (!file) {
        return;
    }

    
    fseek(file, 0, SEEK_END);
    off_t file_size;
    file_size = ftello(file);
    rewind(file);

 

    string total1= string("f")+int_String2(x,10)+int_String2(y,10)+int_String(name2.size(),10)+ name2+  int_String3(file_size,20);


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
void transpuestaAbin(string nameBin,string nameBinT,long filas,long columnas){
	int fd=open(nameBin.c_str(),O_RDONLY);
	if(fd<0) return;

	struct stat st;

	fstat(fd,&st);

	size_t size=st.st_size;

	double* data=(double*) mmap(NULL,size,PROT_READ,MAP_PRIVATE,fd,0);
	

	ofstream salida(nameBinT,ios::binary);
	

	double valor;
	for(long c=0;c<columnas;c++){
		for(long f=0;f<filas;f++){
			valor=data[f*columnas+c];
			salida.write((char*)&valor,sizeof(double));
		}
	}

	salida.close();
	munmap(data,size);
	close(fd);
}
void csv_bin(string nameCsv,string nameBin,long filas,long columnas){
    ifstream csv(nameCsv);
    ofstream bin(nameBin,ios::binary);
    string linea;
    long f=0;
    while(f<filas&&getline(csv, linea)){
        stringstream ss(linea);
        string val;

        long c=0;
        while(c<columnas && getline(ss, val,',')){

            double num=stod(val );

            bin.write((char*)&num,sizeof( double));
            c++;}
        
        f++;}
    csv.close();

    bin.close();
}

void multmatrixBin(string matrixA,long filasA,long columnasA,string matrixB,long filasB,long columnasB,string namemulti){
	if(columnasA!=filasB) return;

	int fdA=open(matrixA.c_str(),O_RDONLY);
	struct stat stA; fstat(fdA,&stA);
	double* A=(double*)mmap(NULL,stA.st_size,PROT_READ,MAP_PRIVATE,fdA,0);

	int fdB=open(matrixB.c_str(),O_RDONLY);
	struct stat stB; fstat(fdB,&stB);
	double* B=(double*)mmap(NULL,stB.st_size,PROT_READ,MAP_PRIVATE,fdB,0);

	
	ofstream salida(namemulti,ios::binary);
	salida.seekp(filasA*columnasB*sizeof(double)-1);
	salida.write("",1);
	salida.close();

	int fdC=open(namemulti.c_str(),O_RDWR);
	double* C=(double*)mmap(NULL,filasA*columnasB*sizeof(double),PROT_READ|PROT_WRITE,MAP_SHARED,fdC,0);

	for(long i=0;i<filasA;i++){
		for(long j=0;j<columnasB;j++){
			double sum=0;
			for(long k=0;k<columnasA;k++){
				sum+=A[i*columnasA+k]*B[k*columnasB+j];
			}
			C[i*columnasB+j]=sum;
		}
	}

	munmap(A,stA.st_size); close(fdA);
	 munmap(B,stB.st_size); close(fdB);

	munmap(C,filasA*columnasB*sizeof(double)); close(fdC);
}




void SUMARbin(vector<string> sumas, long fila, long columna, string guardar)
{
    long totalElem = fila * columna;
    off_t totalBytes = (off_t)totalElem * (off_t)sizeof(double);

    vector<int> fds;
    vector<double*> maps;

    
    for( size_t i=0;i<sumas.size();i++)
    {
        int fd = open(sumas[i].c_str(), O_RDONLY);
        if(fd < 0){
            
            return;
        }

        double* ptr = (double*)mmap(NULL, totalBytes, PROT_READ, MAP_PRIVATE,fd,0);

        if(ptr == MAP_FAILED){
            return;
        }

        fds.push_back(fd);
        maps.push_back(ptr);
    }

    
    int fdOut = open(guardar.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    if(fdOut < 0){
        return;
    }

    
    if(ftruncate(fdOut, totalBytes) != 0){
        close(fdOut);
        return;
    }

    
    double* outPtr=(double*)mmap(NULL,totalBytes,PROT_READ | PROT_WRITE,MAP_SHARED,fdOut,0);

    if(outPtr == MAP_FAILED){
        close(fdOut);
        return;
    }


    for(long f=0; f<fila; f++)
    {
        for(long c= 0;c<columna; c++)
        {
            long idx= f*columna + c;

            double suma=0.0;

            for(double*ptr:maps)
            {
                suma =suma+ ptr[idx];
            }

            outPtr[idx]=suma;   
        }
    }

    
    munmap(outPtr,totalBytes);
    close(fdOut);

    for(size_t i=0; i<maps.size(); i++){
        munmap(maps[i], totalBytes);
        close(fds[i]);
    }
}

void recive_sumas(int sock,int cantsum,string namecapta){
    vector <string> namesworksum;
    long filas;
    long columnas;
    char texto2[2];
    for(int i=0;i<cantsum;i++){
        int n=read(sock,texto2,1);
            if (n<=0){
                 printf("2dbreack\n");
                 fflush(stdout);
                break;
            }
                if (texto2[0]=='f'){
                    printf("entro suma file\n");
                    fflush(stdout);
                    long x=numero_read2(sock,10);
                    filas=x;
                    long y=numero_read2(sock,10);
                    columnas=y;
                    int t4= numero_read(sock,10);

                    string nameF= texto_read(sock,t4);
                    
                    off_t t2= numero_read3(sock,20);
                    
                    
                    //recv_File(int sock,off_t tamano,string nombre_archivo,string carpeta) 
                    recv_File(sock,t2,nameF,namecapta+"/SUMG") ;
                    namesworksum.push_back(namecapta+"/SUMG"+"/"+nameF);
                    



                    }
        }
    printf("entro  SUMNAGGEN\n");
    fflush(stdout);
   //SUMARbin(vector<string> sumas, long fila, long columna, string guardar)
   string almacenarsum="sumageneral.bin";
   SUMARbin(namesworksum, filas,columnas,namecapta+"/SUMG/"+almacenarsum);
    printf("SALIO  SUMNAGGEN\n");
    //void enviarFIle(int sock,string name,string name2,long x, long y)
    fflush(stdout);
    
    enviarFIle(sock,namecapta+"/SUMG/"+almacenarsum,"SUMAwork.bin",filas,columnas);
    


}

void  readd(int sock,string name){
    char texto2[2];
    if (!fs::exists(name)) {
        fs::create_directories(name);
    }
    while (1){
        
        int n=read(sock,texto2,1);
        if (n<=0){
            printf("primer breack\n");
            fflush(stdout);
            break;
        }

          else if (texto2[0]=='b'){


            long filas;
            long columnas;
            string nameF;
        
             n=read(sock,texto2,1);
            if (n<=0){
                 printf("2dbreack\n");
                 fflush(stdout);
                break;
            }
                if (texto2[0]=='f'){

                    long x=numero_read2(sock,10);
                    filas=x;
                    long y=numero_read2(sock,10);
                    columnas=y;
                    int t4= numero_read(sock,10);

                    nameF= texto_read(sock,t4);
                    
                    off_t t2= numero_read3(sock,20);
                    
                    string name2="divvv.csv";
                    //recv_File(int sock,off_t tamano,string nombre_archivo,string carpeta) 
                    recv_File(sock,t2,name2,name); 
                    //csv_bin(string nameCsv,string nameBin,long filas,long columnas)
                    string nambin="enviarbin.bin";
                    string namebinario=name+"/"+nambin;
                    csv_bin(name+"/"+name2,namebinario,filas,columnas);
                    
                    //void enviarFIle(int sock,string name,long x, long y)
                    //enviarFIle(sock,namebinario,nambin,filas,columnas);
                    //transpuestaAbin(string nameBin,string nameBinT,long filas,long columnas){
                    string nemetraspuesta= name+"/"+"transpuestabin.bin";
                    transpuestaAbin(namebinario,nemetraspuesta,filas,columnas);
                    string namemultipli= name+"/"+"muplitrans.bin";
                    multmatrixBin(nemetraspuesta,columnas,filas,namebinario, filas,columnas,namemultipli);

                    enviarFIle(sock,namebinario,nambin,filas,columnas);

                    enviarFIle(sock,namemultipli,"muplitrans.bin",columnas,columnas);


                }

          } 
           else if (texto2[0]=='s'){

                printf("entro suma\n");
    
                fflush(stdout);
                int sumasnum=numero_read(sock,5);

                recive_sumas( sock,sumasnum,name);
           }



    }
         

    shutdown(sock, SHUT_RDWR);
    close(sock);
    printf("\n[%s]-> SALIENDO\n",name.c_str());
    
    fflush(stdout);
}

int main(int argc,char* argv[]) {
    
     if (argc < 2) {
        return 1;
    }
    
    int port = stoi(argv[1]);

    int SocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFD < 0) { perror("socket"); return 1; }

    int yes = 1;
    setsockopt(SocketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in stSockAddr{};
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port   = htons(port);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(SocketFD, (sockaddr*)&stSockAddr, sizeof(stSockAddr)) < 0) {
        perror("bind"); close(SocketFD); return 1;
    }
    if (listen(SocketFD, 1) < 0) {
        perror("listen"); close(SocketFD); return 1;
    }

    cout << "wroker esperando ..\n";

    int cont=1;
    while (true) {
        int ConnectFD = accept(SocketFD, nullptr, nullptr);
        if (ConnectFD < 0) { perror("accept"); continue; }
        
        printf("servidor principal concetado\n");
        fflush(stdout);

        thread t(readd, ConnectFD, string("worker")+to_string(port));
        t.detach();
        
        
        
        
        
        
        
    }

    close(SocketFD);
    return 0;
}