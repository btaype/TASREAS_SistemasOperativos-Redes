#pragma once
#include <string>
#include <vector>
class TCPClient {
public:
    TCPClient(const std::string& ip, int port);
    ~TCPClient();
    bool login(const std::string& nombre);
    std::string recibir(int maxBytes = 1024);
    std::pair<int, int> datosentrenamiento(); 
    std::vector<float> recibirMuestra();
    void enviarMatricesPesos(const std::vector<std::vector<std::vector<float>>> &matrices) ;
    std::vector<std::vector<std::vector<float>>> recibirMatricesPesos();
private:
    int sock;
    bool conectado;
};