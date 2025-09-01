
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

using namespace std;

bool salida = 1;
string nick;


int numero_read(int sockt, int cont) {
    char nume[16];                      
    int n = read(sockt, nume, cont);
    if (n <= 0) return 0;
    nume[n] = '\0';
    return atoi(nume);
}

string texto_read(int sockt, int cont) {
    string out;
    out.resize(cont);
    int leidos = 0;
    while (leidos < cont) {
        int n = read(sockt, &out[leidos], cont - leidos);
        if (n <= 0) {
            out.resize(max(0, leidos));
            break;
        }
        leidos += n;
    }
    return out;
}


void readd(int sock) {
    char texto2[2];
    while (1) {
        int n = read(sock, texto2, 1);
        if (n <= 0) {
            salida = 0;
            return;
        }

        if (texto2[0] == 'n') {
            int tamano = numero_read(sock, 2);
            string texto3 = texto_read(sock, tamano);
            cout << "Nickname Entrante: " << texto3 << '\n';
            nick = texto3;
        }

        if (texto2[0] == 'm') {
            int tamano = numero_read(sock, 3);
            string texto3 = texto_read(sock, tamano);
            cout << '<' << nick << '>' << " dice: " << texto3 << '\n';

            if (texto3 == "chau") {
                salida = 0;
                cout << "cerrando thread\n";
                break;
            }
        }
    }

    shutdown(sock, SHUT_RDWR);
    close(sock);
}


bool write_msg(int sock, string msg) {
    char send2[4];
    sprintf(send2, "%03d", (int)msg.size());
    
    string enviar = string("m") + string(send2) + msg;
    printf("write:  %s\n", enviar.c_str());
    int n = write(sock, enviar.c_str(), (int)enviar.size());
    if (n < 0) return 0;
    return 1;
}

int main() {
    int SocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFD < 0) { perror("socket"); return 1; }

    int yes = 1;
    setsockopt(SocketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in stSockAddr{};
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port   = htons(1100);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(SocketFD, (sockaddr*)&stSockAddr, sizeof(stSockAddr)) < 0) {
        perror("bind"); close(SocketFD); return 1;
    }
    if (listen(SocketFD, 1) < 0) {
        perror("listen"); close(SocketFD); return 1;
    }

    cout << "Servidor esperando en puerto 1100...\n";

    
    while (true) {
        int ConnectFD = accept(SocketFD, nullptr, nullptr);
        if (ConnectFD < 0) { perror("accept"); continue; }

        salida = 1;
        nick.clear();

        
        thread p1(readd, ConnectFD);
        p1.detach();

        
        string miNick;
        cout << "Tu Nick (servidor): ";
        
        if (cin.peek() == '\n') cin.ignore();
        getline(cin, miNick);
       
        
            char num[4];
            sprintf(num, "%02d", (int)miNick.size());
            string send1 = string("n") + string(num) + miNick;
            write(ConnectFD, send1.c_str(), (int)send1.size());
        

        
        while (salida) {
            string texto;
            cout << "\nmsg: ";
           getline(cin, texto);

            if (texto == "chau") {
               
                write_msg(ConnectFD, texto);
                break;
            }

            bool r = write_msg(ConnectFD, texto);
            if (!r) break;
        }

        shutdown(ConnectFD, SHUT_RDWR);
        close(ConnectFD);

        cout << "Sesion cerrada. Aceptando otro cliente...\n";
    }

    close(SocketFD);
    return 0;
}
