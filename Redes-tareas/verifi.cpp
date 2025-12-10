#include <fstream>
#include <iostream>
#include <string>

using namespace std;

void bin_a_csv(string nameBin, string nameCsv, long filas, long columnas){
    ifstream bin(nameBin, ios::binary);
    ofstream csv(nameCsv);

    double num;
    for(long f=0; f<filas; f++){
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
double raiz_cuadrada_segura(double numero) {
    if (numero <= 0) return 0;
    
    double x = numero;
    double y = 1;
    double error_minimo = 0.00000000001;
    int intentos = 0;
    
    while ((x - y) > error_minimo) {
        if (intentos > 80) break; // Evitar bucle infinito
        x = (x + y) / 2;
        y = numero / x;
        intentos = intentos + 1;
    }
    return x;
}
int main(){
    string archivoBin="multAVE2.bin";
    string archivoCsv="datos_recuperados.csv";
    long filas=10000;      // reemplaza con tus filas
    long columnas=1000;   // reemplaza con tus columnas
    double uuu=raiz_cuadrada_segura(19957865.99);
    cout<<uuu<<"\n";
    bin_a_csv(archivoBin, archivoCsv, filas, columnas);

    cout << "Archivo CSV generado: " << archivoCsv << endl;
    printf("El tamaño de double es: %zu bytes\n", sizeof(double));

    return 0;
}
