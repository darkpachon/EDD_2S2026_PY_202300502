#ifndef SOLICITUD_H
#define SOLICITUD_H
#include <string>
using namespace std;

class Solicitud {
public:
    string cliente;
    string tipo; // Ej: "Silla de ruedas", "Subtitulos"

    Solicitud(string _cliente, string _tipo) {
        cliente = _cliente;
        tipo = _tipo;
    }
    Solicitud() {}
};
#endif