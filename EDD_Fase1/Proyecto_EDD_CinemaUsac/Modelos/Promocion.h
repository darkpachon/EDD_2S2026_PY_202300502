#ifndef PROMOCION_H
#define PROMOCION_H
#include <string>
using namespace std;

class Promocion {
public:
    int id;
    string descripcion;
    int porcentajeDescuento;

    Promocion(int _id, string _descripcion, int _porcentaje) {
        id = _id;
        descripcion = _descripcion;
        porcentajeDescuento = _porcentaje;
    }
    Promocion() : id(0), porcentajeDescuento(0) {}
};
#endif