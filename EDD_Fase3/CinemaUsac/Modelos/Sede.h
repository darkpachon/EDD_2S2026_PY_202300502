#ifndef SEDE_H
#define SEDE_H
#include <string>
using namespace std;

class Sede {
public:
    string codigo;
    string nombre;
    string direccion;
    int cantidadSalas;
    string telefono;

    Sede() : cantidadSalas(0) {}
    Sede(string _codigo, string _nombre, string _direccion,
         int _cantidadSalas, string _telefono)
        : codigo(_codigo), nombre(_nombre), direccion(_direccion),
          cantidadSalas(_cantidadSalas), telefono(_telefono) {}
};

#endif