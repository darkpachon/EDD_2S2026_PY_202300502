#ifndef NODOARBOL_H
#define NODOARBOL_H

#include "../Modelos/Pelicula.h"

// Nodo individual del árbol binario de búsqueda de películas.
class NodoArbol {
public:
    Pelicula pelicula;
    NodoArbol* izquierdo;
    NodoArbol* derecho;
    // Guarda una película y comienza sin hijos.
    NodoArbol(Pelicula _pelicula) {
        pelicula = _pelicula;
        izquierdo = nullptr;
        derecho = nullptr;
    }
};

#endif