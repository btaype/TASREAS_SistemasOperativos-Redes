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
    string linea;

    vector<Nodo>nodos;
     string lineTemp;
    ifstream file(archivo);
    
    bool abierto = file.is_open();
    if(!abierto){
        cout<< "sliendo por no abieroo\n";
        return vector<Nodo>(0);

    }

   
    while (1) {

        if(!getline(file,lineTemp)) {
            
            break;
        
        }
        cout<< "ddddeds\n";

        istringstream ss(lineTemp);
        string ip,puerto;

        if (!getline(ss,ip,',')) {continue; }

        if (!getline(ss,puerto,',')) { continue;}

        int port=std::stoi(puerto);

         nodos.push_back({ip,port});}

    return nodos;
}
void conecicion_workers(vector<Nodo> nodos) {
     printf("conectados323232312\n");
     fflush(stdout);
     cout<<nodos.size()<<"   \n";
    for (size_t i=0;i<nodos.size();i++) {
        printf("conectados34342\n");
        fflush(stdout);
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
        
         printf("conectados\n");
        fflush(stdout);
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
    bool exite=fs::exists(rutafin);
    if (exite==false ) {
        fs::create_directories(rutafin);
    }

    long filasPorParte=numlineas/div;


    long filasExtra=numlineas%div;

    ifstream archivo(matrix);

    string linea;

    for(int parte=0;parte<div; parte++){

        long filasActual;
        int pos = parte;
        if(pos< filasExtra){

            filasActual=filasPorParte+1;
        }
        else{
            filasActual =filasPorParte;

        }
        string num= to_string(pos + 1);

        string nombreArchivo= carpetaB+"/"+carpetadiv+"/parte_"+num+".csv";
 
        rutas.push_back( nombreArchivo);


        ofstream salida(nombreArchivo);

        for(long i=0;i< filasActual; i++){
           
            if(!(getline(archivo,linea))){
                break;}

            salida<<linea<<'\n';
            
        }
        filas_a[pos]=filasActual;

        salida.close();
    }

    archivo.close();
    result.rutas=rutas;

    result.filas=filas_a;

    return  result;

}



void enviarFIle(int sock,string name,string name2,long x, long y){
    cout<<"ENMVIE1\n"; 
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


    size_t tamanoEnvio=1024*1024; 
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
     cout<<"ENMVIE2\n";     
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
            name1=carpeta+"/"+nuevoNombre;

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

void juntarBin(vector<string> nombreArchBin,string carpeta_guardar,string nameee2){
	string nombreSalida=carpeta_guardar+"/"+nameee2;

	ofstream salida(nombreSalida,ios::binary|ios::out);

	if(!salida.is_open()) {
        
        return;
    }

	char buffer[1024*1024];

	for(size_t i=0;i<nombreArchBin.size();i++){

		ifstream entrada(nombreArchBin[i],ios::binary);

		if((entrada.is_open())==false ) 
        {continue;
        
        }

		while(1){
            if (entrada.eof()){
                break;

            }
			entrada.read(buffer,sizeof(buffer));

			streamsize leidos=entrada.gcount();

			 if(leidos>0) 
         
            {
            salida.write(buffer,leidos);

            }
		}
	entrada.close();
	}
	salida.close();
}

resultado_div divsion_bin(string matrix,string carpetaM,string carpetaB,string carpetadiv,int div,long numlineas,long numColumnas){
	vector<string> rutas;
	vector<long> filas_a(div,0);

    resultado_div result;
	string rutafin=carpetaB+"/"+carpetadiv;
    
	if(fs::exists(rutafin)==false) {
        fs::create_directories(rutafin);

    }

	long filasPorParte=numlineas/div;
	long filasExtra=numlineas%div;

	int fd=open( (carpetaM+"/"+matrix).c_str(),O_RDONLY);
	struct stat st; fstat(fd,&st);

	double* data =(double*)mmap(NULL,st.st_size,PROT_READ,MAP_PRIVATE,fd,0);
     long filaGlobal = 0;

	for(int parte=0;parte<div;parte++){

		long filasActual;

        if(parte<filasExtra) {

            filasActual=filasPorParte+1;}

        else {
            filasActual=filasPorParte;}

		string num= to_string(parte+1);

		string nombreArchivo=carpetaB+"/"+carpetadiv+"/parte_"+matrix+num+".bin";


		rutas.push_back(nombreArchivo);

		ofstream salida(nombreArchivo, ios::binary);
		for(long i=0; i<filasActual; i++){
            for(long j=0; j<numColumnas; j++){
                long idx = (filaGlobal + i) * numColumnas + j;
                double valor = data[idx];
                salida.write((char*)&valor, sizeof(double));
            }
        }
		filas_a[parte]=filasActual;

        filaGlobal += filasActual; 
		salida.close();
	}

	munmap(data,st.st_size);
	close(fd);

	
	result.rutas=rutas;

	result.filas=filas_a;

	return result;
}

vector<resultado_div> division_bin_mmap(vector<string> archivos ,string carpetaM, string carpetaB, string carpetadiv, int div, long numlineas, long numColumnas){
	vector<resultado_div> todasRutas;

	for(size_t i=0;i<archivos.size();i++){
		resultado_div res=divsion_bin(archivos[i],carpetaM,carpetaB,carpetadiv,div,numlineas,numColumnas);
		todasRutas.push_back(res);
	}

	return todasRutas;
}


void divsumas(int sock,vector<string> namesfiles,vector<long> filasnames,long columna,string carpAlcamacena,int worker,string&namefinall){
    
    int n;
    string accionsum="s"+int_String(namesfiles.size(),5);//---->nuevo portocolo 2 sumas
    write (sock, accionsum.data(),6);
    for (size_t i=0; i<namesfiles.size();i++){
        //enviarFIle(int sock,string name,string name2,long x, long y)

        enviarFIle(sock,namesfiles[i],"suma"+to_string(i)+to_string(sock)+".bin",filasnames[i],columna);


    }
    char texto2[2];

    n=read(sock,texto2,1);
    if (texto2[0]=='f'){

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
            
            //transpuestas[sockt][i]=carpeta+"/"+nuevoNombre;
            namefinall=carpAlcamacena+"/"+nuevoNombre;


    }


}
double raiz_cuadrada_segura(double numero) {
    if (numero <= 0) return 0;
    
    double x = numero;
    double y = 1;
    double error_minimo = 0.00000000001;
    int intentos = 0;
    
    while ((x - y) > error_minimo) {
        if (intentos > 50) break; 
        x = (x + y) / 2;
        y = numero / x;
        intentos = intentos + 1;
    }
    return x;
}
void diagonalinv(string name_archivo,long nn, string nuevo_inv) {

    
    int fd = open(name_archivo.c_str(), O_RDWR);

    if(fd<0) 
    {
        return;
    }
    struct stat st; 
    if(fstat(fd,&st)!=0)
    { close(fd); 
        return; 
    }
    size_t bytes = (size_t)st.st_size;
    double *data = (double*)mmap(NULL, bytes, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(data==MAP_FAILED){
         close(fd);
         return; }

    
    int fd_out=open(nuevo_inv.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd_out< 0) {
        
    munmap(data,st.st_size);
        close(fd);
        return;
    }

    
    if (ftruncate(fd_out,st.st_size) != 0) {
        munmap(data,st.st_size);
        close(fd);
        close(fd_out);
        return;
    }

    
    double* outPtr = (double*) mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_out, 0);
    if (outPtr == MAP_FAILED) {
        munmap(data, st.st_size);
        close(fd);
        close(fd_out);
        return;
    }

    
    
    for (long i = 0; i < nn; i++) {
       double operacion=raiz_cuadrada_segura(data[i*nn+i]);
        data[i*nn+i]=  operacion;
        if (operacion==0){

            outPtr [i*nn+i] = double(0);

        }
        else {
             outPtr [i*nn+i]= 1/operacion;
        }
       
    }

    




    munmap(outPtr, st.st_size);
    munmap(data, st.st_size);
    close(fd_out);
    close(fd);

    
}
string comletreicivirfile(int sock,long &filas, long &columnas, int worker,string carpAlcamacena){


    char texto2[2];

    int n=read(sock,texto2,1);
    if (texto2[0]=='f'){

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
            
            //transpuestas[sockt][i]=carpeta+"/"+nuevoNombre;
            string namefinall=carpAlcamacena+"/"+nuevoNombre;
            return namefinall;

    }

    return "";

}
void enviarAiE_1V(int  sock,string Ai,long filas, long columnas,string E1,string v,int worker,string &nombrefin,string carpeta){
    if (worker ==0){
        string enviar="U";
        write (sock,enviar.data(),1);
         //enviarFIle(int sock,string name,string name2,long x, long y)
        enviarFIle(sock,Ai,"Ai"+to_string(worker)+".bin",filas, columnas);
        enviarFIle(sock,E1,"E1"+to_string(worker)+".bin",columnas, columnas);
        enviarFIle(sock,v,"v"+to_string(worker)+".bin",columnas ,columnas);
        //string comletreicivirfile(int sock,long &filas, long &columnas, int worker,string carpeta)
        long columna1;
        long fila1;
        nombrefin=comletreicivirfile( sock,fila1,columna1, worker,carpeta);
        string nombreVi=comletreicivirfile( sock,fila1,columna1, worker,carpeta);
    }

    else {

        string enviar="u";
        write (sock,enviar.data(),1);
         //enviarFIle(int sock,string name,string name2,long x, long y)
        enviarFIle(sock,Ai,"Ai"+to_string(worker)+".bin",filas, columnas);
        enviarFIle(sock,E1,"E1"+to_string(worker)+".bin",columnas,columnas);
        enviarFIle(sock,v,"v"+to_string(worker)+".bin",columnas ,columnas);
        long columna1;
        long fila1;
        nombrefin=comletreicivirfile( sock,fila1,columna1, worker,carpeta);
    }

}
// Funcion simple para valor absoluto
double valor_absoluto(double numero) {
    if (numero < 0) {
        return -numero;
    }
    return numero;
}

// Funcion para raiz cuadrada con limite de seguridad


// Algoritmo Gram-Schmidt ejecutado localmente en el Master
void gram_schmidt_local(string ruta_A, string ruta_Q, string ruta_R, long N) {
    // Primero copiamos A dentro de Q
    fs::copy_file(ruta_A, ruta_Q, fs::copy_options::overwrite_existing);

    // Abrimos Q para lectura y escritura
    int desc_Q = open(ruta_Q.c_str(), O_RDWR);
    double* matriz_Q = (double*)mmap(NULL, N * N * sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, desc_Q, 0);

    // Creamos R vacia (llena de ceros)
    FILE* archivo_R = fopen(ruta_R.c_str(), "wb");
    // Truco para reservar espacio rapido
    fseek(archivo_R, N * N * sizeof(double) - 1, SEEK_SET); 
    fputc(0, archivo_R); 
    fclose(archivo_R);
    
    int desc_R = open(ruta_R.c_str(), O_RDWR);
    double* matriz_R = (double*)mmap(NULL, N * N * sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, desc_R, 0);

    // Iteramos columna por columna
    for (long i = 0; i < N; i++) {
        double norma = 0;
        
        // Calcular norma de la columna
        for (long k = 0; k < N; k++) {
            double valor = matriz_Q[k * N + i];
            norma = norma + (valor * valor);
        }
        norma = raiz_cuadrada_segura(norma);
        
        // Guardar en la diagonal de R
        matriz_R[i * N + i] = norma;

        // Normalizar la columna en Q
        if (norma > 0.000000001) {
            for (long k = 0; k < N; k++) {
                matriz_Q[k * N + i] = matriz_Q[k * N + i] / norma;
            }
        }

        // Ortogonalizar las siguientes columnas
        for (long j = i + 1; j < N; j++) {
            double producto_punto = 0;
            
            // Calcular la proyeccion (sombra)
            for (long k = 0; k < N; k++) {
                double val_q = matriz_Q[k * N + i];
                double val_sig = matriz_Q[k * N + j];
                producto_punto = producto_punto + (val_q * val_sig);
            }
            matriz_R[i * N + j] = producto_punto;

            // Restar la proyeccion
            for (long k = 0; k < N; k++) {
                double val_restar = producto_punto * matriz_Q[k * N + i];
                matriz_Q[k * N + j] = matriz_Q[k * N + j] - val_restar;
            }
        }
    }
    
    // Cerrar todo
    munmap(matriz_Q, N * N * sizeof(double)); 
    close(desc_Q);
    munmap(matriz_R, N * N * sizeof(double)); 
    close(desc_R);
}

// Funcion que ejecuta el hilo para enviar trabajo al worker
void hilo_enviar_trabajo(int socket_id, string archivo_pedazo, string archivo_Q, string archivo_salida, long filas, long cols, long cols_Q) {
    // Mandamos la señal 'm' para indicar multiplicacion
    write(socket_id, "m", 1); 

    // Enviamos los dos archivos necesarios
    enviarFIle(socket_id, archivo_pedazo, "pedazo.bin", filas, cols);
    enviarFIle(socket_id, archivo_Q, "matriz_Q.bin", cols, cols_Q);

    // Esperamos la respuesta
    char cabecera[2]; 
    read(socket_id, cabecera, 1);
    
    if (cabecera[0] == 'f') {
        long f = numero_read2(socket_id, 10);
        long c = numero_read2(socket_id, 10);
        int largo_nombre = numero_read(socket_id, 10);
        string nombre_temp = texto_read(socket_id, largo_nombre);
        off_t tamano = numero_read3(socket_id, 20);
        
        // Guardamos el resultado en la carpeta del master
        recv_File(socket_id, tamano, archivo_salida, "."); 
    }
}

void crear_matriz_identidad(string ruta_archivo, long N) {
    FILE* archivo = fopen(ruta_archivo.c_str(), "wb");
    double* fila_temp = new double[N];
    
    for (long i = 0; i < N; i++) {
        for (long j = 0; j < N; j++) {
            if (i == j) {
                fila_temp[j] = 1.0;
            } else {
                fila_temp[j] = 0.0;
            }
        }
        fwrite(fila_temp, sizeof(double), N, archivo);
    }
    
    delete[] fila_temp;
    fclose(archivo);
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

    
    for (auto& h : hilos) {
        h.join();
    }
    //juntarBin(nombresBIN,carpeta);
    //vector<resultado_div> division_bin_mmap(vector<string> archivos ,string carpetaM, string carpetaB, string carpetadiv, int div, long numlineas, long numColumnas){
    string namesuma="sumas";
    vector<resultado_div> summ=division_bin_mmap(namemult ,carpeta+"/"+carpetadivbin, carpeta, namesuma, orden.size(), columnas, columnas);
    vector<thread> hilos2;
    vector<string> nombreSUMAG(orden.size());

    string carpetsum=carpeta+"/"+"SUMAG";
    for (auto& p : orden) {
        int socket = p.second;
        vector<string> names;
        vector<long> filasSum;

        for(size_t i=0;i<summ.size();i++){
            names.push_back(summ[i].rutas[p.first]);
            filasSum.push_back(summ[i].filas[p.first]);

        }
        
        hilos2.emplace_back(divsumas, socket,names,filasSum,columnas,carpetsum,p.first,ref(nombreSUMAG[p.first]));

    }

    
    for (auto& h : hilos2) {
        h.join();
    }

    juntarBin(nombreSUMAG,carpeta,"Gsum.bin");

    string nombre_BIN_G=carpeta+"/Gsum.bin";
    
    //koko tu coidgoo aqui el QR,..
    string matriz_A_actual = "qr_A.bin";
    string matriz_V_actual = "qr_V.bin";
    string matriz_Q_actual = "qr_Q.bin";
    string matriz_R_actual = "qr_R.bin";
    string matriz_A_siguiente = "qr_Anext.bin";
    string matriz_V_siguiente = "qr_Vnext.bin";
    
    // Calculamos el tamaño N basandonos en el archivo generado anteriormente
    struct stat info_G; 
    if (stat(nombre_BIN_G.c_str(), &info_G) == 0) {
        long bytes_totales = info_G.st_size;
        // La matriz es cuadrada, asi que N es la raiz de (bytes / 8)
        long N = (long)raiz_cuadrada_segura(bytes_totales / 8);
        
        cout << "\n=== INICIANDO CALCULO QR (Matriz " << N << "x" << N << ") ===" << endl;

        // Copiamos la matriz de entrada para no dañarla
        fs::copy_file(nombre_BIN_G, matriz_A_actual, fs::copy_options::overwrite_existing);
        
        // Creamos la matriz identidad para empezar a calcular autovectores
        crear_matriz_identidad(matriz_V_actual, N);

        int maximo_iteraciones = 2; 
        int total_workers = orden.size(); 

        for (int iter = 0; iter < maximo_iteraciones; iter++) {
            cout << "Iteracion QR " << iter + 1 << "..." << endl;

            // Paso 1: Descomposicion QR local en el Master
            gram_schmidt_local(matriz_A_actual, matriz_Q_actual, matriz_R_actual, N);

            // Paso 2: Multiplicacion Distribuida
            // Hacemos dos pasadas: 
            // - Pasada 0: Calculamos A nueva (Autovalores)
            // - Pasada 1: Calculamos V nueva (Autovectores)
            
            for (int paso = 0; paso < 2; paso++) {
                string archivo_entrada;
                string archivo_salida;
                
                // Seleccionamos que archivos usar sin operadores raros
                if (paso == 0) {
                    archivo_entrada = matriz_R_actual;
                    archivo_salida = matriz_A_siguiente;
                } else {
                    archivo_entrada = matriz_V_actual;
                    archivo_salida = matriz_V_siguiente;
                }
                
                // Mapear el archivo de entrada para cortarlo en pedazos
                int desc_in = open(archivo_entrada.c_str(), O_RDONLY);
                double* matriz_in = (double*)mmap(NULL, N * N * sizeof(double), PROT_READ, MAP_PRIVATE, desc_in, 0);

                vector<thread> lista_hilos;
                vector<string> archivos_parciales;
                
                long filas_por_worker = N / total_workers;
                long filas_extra = N % total_workers;
                long fila_actual = 0;

                // Repartir trabajo a los workers conectados
                for (auto const& [id, socket_worker] : orden) {
                    long filas_este_worker = filas_por_worker;
                    
                    // Repartir el residuo entre los primeros
                    if (id < filas_extra) {
                        filas_este_worker = filas_este_worker + 1;
                    }
                    
                    // Crear un archivo pequeño con el pedazo de matriz
                    string nombre_pedazo = "pedazo_" + to_string(id) + ".bin";
                    FILE* archivo_temp = fopen(nombre_pedazo.c_str(), "wb");
                    
                    for (long r = 0; r < filas_este_worker; r++) {
                        long indice_global = (fila_actual + r) * N;
                        fwrite(&matriz_in[indice_global], sizeof(double), N, archivo_temp);
                    }
                    fclose(archivo_temp);

                    string salida_worker = "salida_w_" + to_string(id) + ".bin";
                    archivos_parciales.push_back(salida_worker);

                    // Lanzar el hilo para este worker
                    lista_hilos.emplace_back(hilo_enviar_trabajo, socket_worker, nombre_pedazo, matriz_Q_actual, salida_worker, filas_este_worker, N, N);
                    
                    fila_actual = fila_actual + filas_este_worker;
                }
                
                // Esperar a que todos terminen
                for (auto& t : lista_hilos) {
                    t.join();
                }
                
                munmap(matriz_in, N * N * sizeof(double)); 
                close(desc_in);

                // Juntar todos los pedacitos en un solo archivo
                FILE* archivo_final = fopen(archivo_salida.c_str(), "wb");
                for (string nombre_parcial : archivos_parciales) {
                    int desc_p = open(nombre_parcial.c_str(), O_RDONLY);
                    struct stat info_p; 
                    fstat(desc_p, &info_p);
                    
                    double* datos_p = (double*)mmap(NULL, info_p.st_size, PROT_READ, MAP_PRIVATE, desc_p, 0);
                    fwrite(datos_p, 1, info_p.st_size, archivo_final);
                    
                    munmap(datos_p, info_p.st_size); 
                    close(desc_p);
                }
                fclose(archivo_final);
            } 

            // Paso 3: Verificar convergencia (Error)
            int desc_check = open(matriz_A_siguiente.c_str(), O_RDONLY);
            double* A_verificar = (double*)mmap(NULL, N * N * sizeof(double), PROT_READ, MAP_PRIVATE, desc_check, 0);
            
            double error_total = 0;
            for (long i = 0; i < N; i++) {
                for (long j = 0; j < N; j++) {
                    if (i != j) {
                        error_total = error_total + valor_absoluto(A_verificar[i * N + j]);
                    }
                }
            }
            munmap(A_verificar, N * N * sizeof(double)); 
            close(desc_check);
            
            cout << "   -> Error actual: " << error_total << endl;

            // Actualizar archivos para la siguiente vuelta
            fs::copy_file(matriz_A_siguiente, matriz_A_actual, fs::copy_options::overwrite_existing);
            fs::copy_file(matriz_V_siguiente, matriz_V_actual, fs::copy_options::overwrite_existing);

            if (error_total < 0.00001) break;

        }
        
        cout << "=== FIN DEL PROCESO QR ===" << endl;
        cout << "Autovalores guardados en: " << matriz_A_actual << endl;
        cout << "Autovectores guardados en: " << matriz_V_actual << endl;
    }




    //E y U y V
    string diagoanl=matriz_A_actual;
    string diagonalinv1=carpeta+"/invdiagonal.bin";
    string autovectores= matriz_V_actual;


    diagonalinv(diagoanl,filas,diagonalinv1);
    vector<thread> hilos3;
    vector<string> nombreU(orden.size());

    
    for (auto& p : orden) {
        int socket = p.second;
        string archivo =namepart[p.first];
        long fil= particiones.filas[p.first];
        //enviarAiE_1V(int  sock,string Ai,long filas, long columnas,string E1,string v,int worker,string&nombrefin,string carpeta)
        //enviarAiE_1V(int  sock,string Ai,long filas, long columnas,string E1,string v,int worker,string&nombrefin,string carpeta)
        hilos3.emplace_back(enviarAiE_1V, socket,archivo,fil,columnas,diagonalinv1, autovectores,p.first,ref(nombreU[p.first]),carpeta);

    }

     for (auto& h : hilos3) {
        h.join();
    }

    juntarBin(nombreU,carpeta,"U.bin");

    string archivoU=carpeta+"/"+"U.bin";
    string VVV=carpeta+"/"+"traspV0.bin";
    #include <filesystem>
    if (!std::filesystem::exists(VVV)) {
        std::cerr << "Error: archivo " << VVV << " no existe\n";
    }
    if (!std::filesystem::exists(archivoU)) {
        std::cerr << "Error: archivo " << archivoU << " no existe\n";
    }
    enviarFIle(sock,archivoU,"Ufin.bin",columnas,columnas);
    enviarFIle(sock,diagoanl,"Efinal.bin",columnas,columnas);

    enviarFIle(sock,VVV,"Vfinal.bin",columnas,columnas);

   
    
    n=read(sock,texto2,1);
    if (n<=0){
        return;
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