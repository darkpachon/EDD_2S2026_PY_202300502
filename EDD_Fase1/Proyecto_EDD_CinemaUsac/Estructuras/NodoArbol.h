#ifndef NODOARBOL_H
#define NODOARBOL_H

#include "../Modelos/Pelicula.h"

class NodoArbol {
public:
    Pelicula pelicula;
    NodoArbol* izquierdo;
    NodoArbol* derecho;

    NodoArbol(Pelicula _pelicula) {
        pelicula = _pelicula;
        izquierdo = nullptr;
        derecho = nullptr;
    }
};

#endif