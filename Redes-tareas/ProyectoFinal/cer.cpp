#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr{}, client_addr{};
    socklen_t client_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error al crear el socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7003);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 1) < 0) {
        perror("Error en listen");
        close(server_fd);
        return 1;
    }

    std::cout << "[Servidor] Esperando conexión en el puerto 7002...\n";

    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("Error en accept");
        close(server_fd);
        return 1;
    }
    char buffer[11];
    std::cout << "[Servidor] Cliente conectado: "
              << inet_ntoa(client_addr.sin_addr) << "\n";
    ssize_t leidos = read(client_fd, buffer, 10);
    printf("%s\n",buffer);
    std::string a = "A002001";
    ssize_t enviados = write(client_fd, a.c_str(), a.size());
    std::string b = "D000200043.140004-2.0";
    ssize_t enviados1 = write(client_fd, b.c_str(), b.size());

    std::string c = "D000100010";
    ssize_t enviados2 = write(client_fd, c.c_str(), c.size());
    char buffer2[1001]; // o más grande
    leidos = read(client_fd, buffer2, 1000);
 
    

    if (leidos > 0) {
        buffer2[leidos] = '\0'; // Asegurar terminación si es texto
        std::cout << "[Cliente] Leídos " << leidos << " bytes\n";
        std::cout << "[Cliente] Contenido:\n" << buffer2 << std::endl;}
    std::string v="m0004p000400020012-0.2222396220012-0.1688555480012-0.2751574520012-0.10651141400110.10835224400120.09895340350012-0.6989237670012-0.419905663p0004000400110.47810107500110.2185173030012-0.3262984750012-0.1675100920012-0.37381070900110.4652846460012-0.11622691200110.1848799590012-0.2185213570012-0.3084985610012-0.15053385500110.14155232900100.3786076900110.43000847100110.3119772370013-0.0860831141p0004000400110.0962806940011-0.271578610013-0.01971662040012-0.3935278060012-0.3696009520012-0.4527577760012-0.4399780630012-0.49916672700110.2407640220012-0.3666881920012-0.2391024830012-0.2334216240012-0.33781397300110.19761991500130.0051096677800110.437029481p0001000400110.28449851300110.4752113220012-0.27405893800110.133977026";
     ssize_t enviados3 = write(client_fd, v.c_str(), v.size());
   
    char fw[2] = {'F', '\0'};
    write(client_fd, fw,1);
   
     if (enviados < 0) {
        perror("Error al enviar con write");
    } else {
        std::cout << "[Servidor] Enviado: " << a << std::endl;
    }
    while (true){

    }
    close(client_fd);
    close(server_fd);

    return 0;
}