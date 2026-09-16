#ifndef SOLICITUD_H
#define SOLICITUD_H
#include <string>
using namespace std;

// Representa una solicitud especial hecha por un cliente.
class Solicitud {
public:
    int numero;
    string cliente;
    string telefono;
    string tipo;  
    string prioridad;
    string descripcion;
    string fecha;
    string estado;

    // Crea una solicitud con estado inicial Pendiente.
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
    // Constructor vacío con valores iniciales válidos.
    Solicitud() : numero(0), prioridad("Normal"), estado("Pendiente") {}
};
#endif