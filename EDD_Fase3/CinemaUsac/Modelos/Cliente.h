#ifndef CLIENTE_H
#define CLIENTE_H
#include <string>
#include <vector>
using namespace std;

class Cliente {
public:
    string id;
    string nombre;
    string correo;
    string telefono;
    string password;
    string tipo;
    vector<string> codigosReserva;

    Cliente() : tipo("cliente") {}

    Cliente(string _id, string _nombre, string _correo, string _telefono,
            string _password, string _tipo = "cliente")
        : id(_id), nombre(_nombre), correo(_correo), telefono(_telefono),
          password(_password), tipo(_tipo) {}
};

#endif