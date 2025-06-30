#include <pybind11/pybind11.h>
#include "tcp_client.hpp"
#include <pybind11/stl.h>  
namespace py = pybind11;

static TCPClient* cliente = nullptr;

PYBIND11_MODULE(comunicacion, m) {
    m.def("iniciar_cliente", [](std::string ip, int puerto) {
        if (cliente) delete cliente;
        cliente = new TCPClient(ip, puerto);
    });

    m.def("login", [](std::string nombre) {
        if (!cliente) throw std::runtime_error("Cliente no iniciado");
        return cliente->login(nombre);
    });
    m.def("datosentrenamiento", []() {
    if (!cliente) throw std::runtime_error("Cliente no iniciado");
    return cliente->datosentrenamiento();
});
    m.def("recibir", [](int maxBytes) {
        if (!cliente) throw std::runtime_error("Cliente no iniciado");
        return cliente->recibir(maxBytes);
    });
    m.def("recibirMuestra", []() {
    if (!cliente) throw std::runtime_error("Cliente no iniciado");
    return cliente->recibirMuestra();  // list[float] en Python
});

    m.def("enviar_matricesPesos", [](const std::vector<std::vector<std::vector<float>>> &matrices) {
    if (!cliente) throw std::runtime_error("Cliente no iniciado");
    cliente->enviarMatricesPesos(matrices);
});
    m.def("recibirMatricesPesos", []() {
        if (!cliente) throw std::runtime_error("Cliente no iniciado");
        return cliente->recibirMatricesPesos();  
    });
}
