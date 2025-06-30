#include "tcp_client.hpp"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <limits>
using namespace std;
TCPClient::TCPClient(const std::string& ip, int port) {
    struct sockaddr_in stSockAddr;
    int Res;

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    conectado = false;

    if (sock == -1) {
        perror("No se pudo crear el socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(port);
    Res = inet_pton(AF_INET, ip.c_str(), &stSockAddr.sin_addr);

    if (Res <= 0) {
        if (Res == 0)
            std::cerr << "Dirección IP inválida\n";
        else
            perror(" Error en inet_pton");

        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (const struct sockaddr*)&stSockAddr, sizeof(struct sockaddr_in)) == -1) {
        perror("Conexión fallida");
        close(sock);
        exit(EXIT_FAILURE);
    }

    conectado = true;
    std::cout << " Conectado al servidor TCP\n";
}

TCPClient::~TCPClient() {
    if (conectado) close(sock);
}

bool TCPClient::login(const std::string& nombre) {
    if (!conectado) return false;
   
    if (nombre.size() > 99) return false; 
    char tam_str[3];
    snprintf(tam_str, sizeof(tam_str), "%02zu", nombre.size());  

    std::string mensaje = "C";
    mensaje += tam_str;
    mensaje += nombre;
    cout<<mensaje<<'\n';
    return write(sock, mensaje.c_str(), mensaje.size()) >= 0;


}
std::pair<int, int> TCPClient::datosentrenamiento() {
    char buffer[7];
    int recibidos = read(sock, buffer, 7);
    if (recibidos != 7) throw std::runtime_error("Error al leer los 7 bytes");

    if (buffer[0] != 'A') throw std::runtime_error("Formato incorrecto: falta 'C'");

    
    std::string str1(buffer + 1, 3);  
    std::string str2(buffer + 4, 3);  

    
    int n1 = std::stoi(str1);
    int n2 = std::stoi(str2);

    return {n1, n2};
}
std::string TCPClient::recibir(int maxBytes) {
    if (!conectado) return "";
    char buffer[1024] = {0};
    int bytes = read(sock, buffer, maxBytes);
    if (bytes <= 0) return "";
    return std::string(buffer, bytes);
}
std::vector<float> TCPClient::recibirMuestra() {
    // Leer solo 1 byte: debe ser 'D'
    char tipo;
    cout<<"entre1\n";
    int leido = read(sock, &tipo, 1);
    cout<<leido<<'\n';
    if (leido != 1 || tipo == 'F' ) {
          return {};
    }

    // Luego leer los 4 caracteres del tamaño de columna
    char columna_buffer[4];
    leido = read(sock, columna_buffer, 4);
    if (leido != 4) {
        throw std::runtime_error(" No se pudieron leer los 4 bytes del tamaño de columna");
    }

    int columnas = std::stoi(std::string(columna_buffer, 4));
    if (columnas <= 0 || columnas > 9999) {
        throw std::runtime_error(" Número de columnas inválido: " + std::to_string(columnas));
    }

    std::vector<float> valores;

    for (int i = 0; i < columnas; ++i) {
        // Leer 4 caracteres del tamaño del dato
        char tam_buffer[4];
        int r1 = read(sock, tam_buffer, 4);
        if (r1 != 4) throw std::runtime_error(" Error al leer tamaño del dato");

        int tam_dato = std::stoi(std::string(tam_buffer, 4));

        

        if (tam_dato <= 0 || tam_dato > 100)//100 para  no tener muchos caracteres ya que un float masximo tiene 17 caracteres 
            throw std::runtime_error("Tamaño de dato inválido");

        // Leer el valor como string
        std::vector<char> data_chars(tam_dato);
        int r2 = read(sock, data_chars.data(), tam_dato);
        if (r2 != tam_dato)
            throw std::runtime_error(" No se pudo leer el dato completo");

        std::string str_valor(data_chars.begin(), data_chars.end());
        try {
            float numero = std::stof(str_valor);
            valores.push_back(static_cast<float>(numero));  // Garantiza float32
        } catch (...) {
            throw std::runtime_error(" Conversión inválida de '" + str_valor + "'");
        }
    }

    return valores;
}



void TCPClient::enviarMatricesPesos(const std::vector<std::vector<std::vector<float>>> &matrices) {
    // 1. Enviar encabezado: "MXXXX"
    if (matrices.size() > 9999)
        throw std::runtime_error("Demasiadas matrices (máximo 9999)");

    char tam_str[5];
    snprintf(tam_str, sizeof(tam_str), "%04zu", matrices.size());
    std::string mensaje = "M" + std::string(tam_str);

    if (write(sock, mensaje.c_str(), mensaje.size()) != (ssize_t)mensaje.size()) {
        throw std::runtime_error("No se pudo enviar encabezado de matrices");
    }

    // 2. Enviar cada matriz
    for (size_t i = 0; i < matrices.size(); ++i) {
        const auto& matriz = matrices[i];

        if (matriz.empty() || matriz[0].empty())
            throw std::runtime_error("Matriz vacía no permitida");

        std::string envio = "P";

        char fila[5], columna[5];
        snprintf(fila, sizeof(fila), "%04zu", matriz.size());
        snprintf(columna, sizeof(columna), "%04zu", matriz[0].size());
        envio += fila;
        envio += columna;

        for (size_t y = 0; y < matriz.size(); ++y) {
            for (size_t u = 0; u < matriz[y].size(); ++u) {
                std::ostringstream oss;
                oss << std::setprecision(std::numeric_limits<float>::max_digits10) << matriz[y][u];
                std::string num = oss.str();

                char tamanoDato[5];
                snprintf(tamanoDato, sizeof(tamanoDato), "%04zu", num.size());

                envio += tamanoDato;
                envio += num;
            }
        }
        cout<<envio<<'\n';
        if (write(sock, envio.c_str(), envio.size()) != (ssize_t)envio.size()) {
            throw std::runtime_error("No se pudo enviar matriz #" + std::to_string(i));
        }
    }
}

std::vector<std::vector<std::vector<float>>> TCPClient::recibirMatricesPesos() {
    // Leer encabezado: 'm' + 4 caracteres de cantidad de matrices
    char encabezado[5];
    int r1 = read(sock, encabezado, 5);
    if (r1 != 5 || encabezado[0] != 'm') {
        throw std::runtime_error("Encabezado inválido o falta 'm'");
    }

    int cantidadMatrices = std::stoi(std::string(encabezado + 1, 4));
    std::vector<std::vector<std::vector<float>>> resultado;

    for (int i = 0; i < cantidadMatrices; ++i) {
        // Leer prefijo 'p'
        char tipo;
        if (read(sock, &tipo, 1) != 1 || tipo != 'p') {
            throw std::runtime_error(" Error: falta prefijo 'p'");
        }

        // Leer filas y columnas (4 caracteres cada uno)
        char filaStr[5] = {0}, colStr[5] = {0};
        if (read(sock, filaStr, 4) != 4 || read(sock, colStr, 4) != 4) {
            throw std::runtime_error(" Error al leer tamaño de fila/columna");
        }

        int filas = std::stoi(std::string(filaStr, 4));
        int columnas = std::stoi(std::string(colStr, 4));
        std::vector<std::vector<float>> matriz(filas, std::vector<float>(columnas));

        for (int f = 0; f < filas; ++f) {
            for (int c = 0; c < columnas; ++c) {
                // Leer longitud del valor (4 caracteres)
                char lenStr[5] = {0};
                if (read(sock, lenStr, 4) != 4) {
                    throw std::runtime_error(" Error al leer tamaño del dato");
                }

                int lenDato = std::stoi(std::string(lenStr, 4));
                std::vector<char> buffer(lenDato);
                if (read(sock, buffer.data(), lenDato) != lenDato) {
                    throw std::runtime_error(" Error al leer el dato");
                }

                std::string strValor(buffer.begin(), buffer.end());
                matriz[f][c] = std::stof(strValor);
            }
        }

        resultado.push_back(matriz);
    }

    return resultado;
}