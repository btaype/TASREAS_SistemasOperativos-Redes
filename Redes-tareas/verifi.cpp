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

int main(){
    string archivoBin="muplitrans.bin";
    string archivoCsv="datos_recuperados.csv";
    long filas=10;      // reemplaza con tus filas
    long columnas=10;   // reemplaza con tus columnas

    bin_a_csv(archivoBin, archivoCsv, filas, columnas);

    cout << "Archivo CSV generado: " << archivoCsv << endl;
    printf("El tamaño de double es: %zu bytes\n", sizeof(double));

    return 0;
}
