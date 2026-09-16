#ifndef PROMOCION_H
#define PROMOCION_H
#include <string>
using namespace std;

// Contiene la información general de una promoción.
class Promocion {
public:
    int id;
    string descripcion;
    int porcentajeDescuento;
    string fechaInicio;
    string fechaFin;
    string diasAplicables;

    // Inicializa el identificador, descripción y porcentaje de descuento.
    Promocion(int _id, string _descripcion, int _porcentaje) {
        id = _id;
        descripcion = _descripcion;
        porcentajeDescuento = _porcentaje;
        fechaInicio = "";
        fechaFin = "";
        diasAplicables = "";
    }
    Promocion() : id(0), porcentajeDescuento(0) {}
};
#endif