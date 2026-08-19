#ifndef ARBOLPELICULAS_H
#define ARBOLPELICULAS_H

#include "NodoArbol.h"
#include <iostream>

class ArbolPeliculas {
private:
    NodoArbol* raiz;
    NodoArbol* insertarRecursivo(NodoArbol* nodoActual, Pelicula nuevaPelicula);
    NodoArbol* buscarRecursivo(NodoArbol* nodoActual, int codigoABuscar);
    void inordenRecursivo(NodoArbol* nodoActual);

public:
    ArbolPeliculas();
    void insertar(Pelicula nuevaPelicula);
    Pelicula* buscar(int codigoABuscar);
    void mostrarInorden();
    NodoArbol* getRaiz(); // Útil para cuando hagamos el reporte en Graphviz
};

#endif // ARBOLPELICULAS_H