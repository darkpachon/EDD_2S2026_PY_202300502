#ifndef CLIENTE_H
#define CLIENTE_H
#include <string>
#include <vector>
using namespace std;

// Cliente registrado. El id es la clave del Árbol B de orden 4.
class Cliente {
public:
    string id;                      // Ej: "u001" -- clave del Árbol B
    string nombre;
    string correo;
    string telefono;
    string password;
    string tipo;                     // "cliente"
    vector<string> codigosReserva;   // codigos_reserva asociados (para ir cliente -> hash)

    Cliente() : tipo("cliente") {}

    Cliente(string _id, string _nombre, string _correo, string _telefono,
            string _password, string _tipo = "cliente")
        : id(_id), nombre(_nombre), correo(_correo), telefono(_telefono),
          password(_password), tipo(_tipo) {}
};

#endif