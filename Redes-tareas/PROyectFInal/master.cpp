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
using namespace std;

namespace fs = std::filesystem;
mutex mtx_maps;
bool salida=1;
constexpr int BUFFER_SIZE = 300;
string nick;
char sym;
string tab="_________";

map <int,vector<string>> nombresBIN;
map <int,vector<string>> transpuestas;

map<int,int>orden;     
map<int,int>listsock;  
int contador = 0;

struct resultado_div {
    vector<string> rutas;
    vector<long> filas;
};

struct Nodo {
    string ip;
    int port;
    

};

vector<Nodo> recibirIpsPorts(string archivo) {
    vector<Nodo>nodos;
    ifstream file(archivo);
    if (!file.is_open()) {
        
        return nodos;
    }

   string linea;
    while (getline(file, linea)) {

    istringstream ss(linea);
        string ip,puerto;

        if (!getline(ss,ip,',')) {continue; }

        if (!getline(ss,puerto,',')) { continue;}

        int port=std::stoi(puerto);

         nodos.push_back({ip,port});}

    return nodos;
}
void conecicion_workers(vector<Nodo> nodos) {
    for (size_t i=0;i<nodos.size();i++) {
        int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

        if (sock_fd < 0) { perror("socket"); return ; }
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(nodos[i].port);
        inet_pton(AF_INET, nodos[i].ip.c_str(), &server_addr.sin_addr);
        if (connect(sock_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("connect"); close(sock_fd); return ;
        }
        int ord=contador;
        orden[ord]=sock_fd;
        listsock[sock_fd]=ord;
        
        cout<< "WORKER-->"<< contador<< "listo\n";
        contador++;
    }

    

}
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
   
    fflush(stdout);
    return total;

}
long numero_read2(int sockt,int cont){
    char nume[1000];
    int n=read (sockt,nume,cont);
    nume[n]='\0';
    return atol(nume);
}


 resultado_div dividir_carpetas( string matrix,string carpetaB,string carpetadiv,int div, long numlineas ) {

    vector<string> rutas;
    string rutafin= carpetaB + "/" + carpetadiv;
    resultado_div result;
    vector<long> filas_a(div, 0);

    if (!fs::exists(rutafin)) {
        fs::create_directories(rutafin);
    }
    long filasPorParte=numlineas/div;


    long filasExtra=numlineas%div;

    ifstream archivo(matrix);

    string linea;

    for(int parte=0;parte<div; parte++){

        long filasActual;

        if(parte< filasExtra){
            filasActual=filasPorParte+1;
        }
        else{
            filasActual =filasPorParte;

        }
        string num= to_string(parte + 1);

        string nombreArchivo= carpetaB+"/"+carpetadiv+"/parte_"+num+".csv";
 
        rutas.push_back( nombreArchivo);


        ofstream salida(nombreArchivo);

        for(long i=0;i< filasActual; i++){

            if(!getline(archivo,linea)){
                break;}

            salida<<linea<<'\n';
            
        }
        filas_a[parte]=filasActual;

        salida.close();
    }

    archivo.close();
    result.rutas=rutas;
    result.filas=filas_a;
    return  result;

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
void enviar_particiones_csv(int sockt,long filas, long columnas,string name,string carpeta, int i,string &name1,string &name2){
    printf("entreee6\n");
    fflush(stdout);
    string accion="b";//protocolo b 
    write (sockt,accion.data(),1);
    enviarFIle(sockt,name,name,filas,columnas);
    char texto2[2];
    int n=read(sockt,texto2,1);
    printf("entreee7\n");
    if (texto2[0]=='f'){
    string nameF;
            long x=numero_read2(sockt,10);
            filas=x;
            long y=numero_read2(sockt,10);
            columnas=y;
            int t4= numero_read(sockt,10);

            nameF= texto_read(sockt,t4);
            
            off_t t2= numero_read3(sockt,20);
            
            size_t pos=nameF.rfind(".bin");
            string nuevoNombre;
            nuevoNombre=nameF.substr(0,pos)+to_string(i)+nameF.substr(pos);
            recv_File(sockt,t2,nuevoNombre,carpeta);
              
            //nombresBIN[sockt][i]=carpeta+"/"+nuevoNombre;
            name1=nuevoNombre;

            //printf("guardado-->%s\n", nombresBIN[sockt][i].data());
            //fflush(stdout);
    }
    n=read(sockt,texto2,1);
    if (texto2[0]=='f'){
    string nameF;
    long x=numero_read2(sockt,10);
            filas=x;
            long y=numero_read2(sockt,10);
            columnas=y;
            int t4= numero_read(sockt,10);

            nameF= texto_read(sockt,t4);
            
            off_t t2= numero_read3(sockt,20);
            
            size_t pos=nameF.rfind(".bin");
            string nuevoNombre;
            nuevoNombre=nameF.substr(0,pos)+to_string(i)+nameF.substr(pos);
            recv_File(sockt,t2,nuevoNombre,carpeta);
            
            //transpuestas[sockt][i]=carpeta+"/"+nuevoNombre;
            name2=nuevoNombre;

            //printf("guardado-->%s\n", transpuestas[sockt][i].data());
            //fflush(stdout);
    } 



}

void juntarBin(vector<string> nombreArchBin,string carpeta_guardar){
	string nombreSalida=carpeta_guardar+"/matrixunion.bin";

	ofstream salida(nombreSalida,ios::binary|ios::out);

	if(!salida.is_open()) return;

	char buffer[1024*1024];

	for(size_t i=0;i<nombreArchBin.size();i++){
		ifstream entrada(nombreArchBin[i],ios::binary);
		if(!entrada.is_open()) continue;

		while(!entrada.eof()){
			entrada.read(buffer,sizeof(buffer));

			streamsize leidos=entrada.gcount();

			 if(leidos>0) salida.write(buffer,leidos);
		}
	entrada.close();
	}
	salida.close();
}

resultado_div divsion_bin(string matrix,string carpetaM,string carpetaB,string carpetadiv,int div,long numlineas,long numColumnas){
	vector<string> rutas;
	vector<long> filas_a(div,0);

	string rutafin=carpetaB+"/"+carpetadiv;
    
	if(!fs::exists(rutafin)) {
        fs::create_directories(rutafin);

    }

	long filasPorParte=numlineas/div;
	long filasExtra=numlineas%div;

	int fd=open( (carpetaM+"/"+matrix).c_str(),O_RDONLY);
	struct stat st; fstat(fd,&st);

	double* data =(double*)mmap(NULL,st.st_size,PROT_READ,MAP_PRIVATE,fd,0);

	for(int parte=0;parte<div;parte++){

		long filasActual=(parte<filasExtra)?(filasPorParte+1):filasPorParte;

		string num= to_string(parte+1);

		string nombreArchivo=carpetaB+"/"+carpetadiv+"/parte_"+matrix+num+".bin";
		rutas.push_back(nombreArchivo);

		ofstream salida(nombreArchivo, ios::binary);
		for(long i=0;i<filasActual;i++){

			for(long j=0;j< numColumnas;j++){

				long idx=( ( (parte<filasExtra?parte*(filasPorParte+1):parte*filasPorParte) + i ) * numColumnas + j );
				double valor=data[idx];

				salida.write((char*)&valor,sizeof(double));
			}
		}
		filas_a[parte]=filasActual;


		salida.close();
	}

	munmap(data,st.st_size);
	close(fd);

	resultado_div result;
	result.rutas=rutas;
	result.filas=filas_a;
	return result;
}

vector<resultado_div> division_bin_mmap(vector<string> archivos, string carpetaB, string carpetadiv, int div, long numlineas, long numColumnas){
	vector<resultado_div> todasRutas;
	for(auto &file:archivos){
		resultado_div res=dividir_bin_mmap(file,carpetaB,carpetadiv,div,numlineas,numColumnas);
		todasRutas.push_back(res);
	}
	return todasRutas;
}
void  readd(int sock){
    //map <int,vector<string>> nombresBIN;
    //map <int,vector<string>> transpuestas;
    
    

    //printf("entreee1\n");
    fflush(stdout);
    char texto2[2];
    
        long filas;
        string nameF;
        long columnas;
        string carpeta=to_string(sock);

        int n=read(sock,texto2,1);
    //printf("entreee2\n");
    fflush(stdout);
        if (n<=0){
            shutdown(sock, SHUT_RDWR);
             close(sock);
           return;
        }
        
          else if (texto2[0]=='f'){

            long x=numero_read2(sock,10);
            filas=x;
            long y=numero_read2(sock,10);
            columnas=y;
            int t4= numero_read(sock,10);

            nameF= texto_read(sock,t4);
            
            off_t t2= numero_read3(sock,20);
            

            recv_File(sock,t2,nameF,carpeta); 

          } 
    //printf("entreee3\n");
    fflush(stdout);
    string nameM=carpeta+"/"+nameF;
    //( string matrix,string carpetaB,string carpetadiv,int div, long numlineas ) 

    string carpetaPaso="divisiones";     
    resultado_div particiones=dividir_carpetas(nameM,carpeta,carpetaPaso,listsock.size(),filas); 
    vector<string> namepart(orden.size());
    vector<string> namemult(orden.size());
    vector<thread> hilos;
    string carpetadivbin="divBin";
    
    for (auto& p : orden) {
        int socket = p.second;
        string archivo = particiones.rutas[p.first];
        long fil= particiones.filas[p.first];
        
        hilos.emplace_back(enviar_particiones_csv,socket,fil, columnas,archivo,carpeta+"/"+carpetadivbin,p.first,ref(namepart[p.first]),ref(namemult[p.first]));

    }

    // Esperar a que todos terminen
    for (auto& h : hilos) {
        h.join();
    }
    //juntarBin(nombresBIN,carpeta);

    while(1){

    }
    shutdown(sock, SHUT_RDWR);
    close(sock);
    
    
    fflush(stdout);
  

    
       
    
}


int main(int argc,char* argv[]) {
    
     if (argc < 2) {
        return 1;
    }
   
    int port=stoi(argv[1]);
    string namefile = "archivoIP.csv"; 
    vector<Nodo>nodos=recibirIpsPorts(namefile );
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

    conecicion_workers( nodos);
    while (true) {
        int ConnectFD = accept(SocketFD, nullptr, nullptr);
        if (ConnectFD < 0) { perror("accept"); continue; }
        //nombresBIN[ConnectFD]=vector<string>(orden.size());
        //transpuestas[ConnectFD]=vector<string>(orden.size());
        thread t(readd, ConnectFD);
        t.detach();


    }

    close(SocketFD);
    return 0;
}