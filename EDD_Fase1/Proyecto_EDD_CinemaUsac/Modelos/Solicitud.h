#ifndef SOLICITUD_H
#define SOLICITUD_H
#include <string>
using namespace std;

class Solicitud {
public:
    int numero;
    string cliente;
    string telefono;
    string tipo; // Ej: "Silla de ruedas", "Subtitulos"
    string prioridad;
    string descripcion;
    string fecha;
    string estado;

    Solicitud(string _cliente, string _tipo) {
        numero = 0;
        cliente = _cliente;
        telefono = "";
        tipo = _tipo;
        prioridad = "Normal";
        descripcion = "";
        fecha = "";
        estado = "Pendiente";
    }
    Solicitud() : numero(0), prioridad("Normal"), estado("Pendiente") {}
};
#endif