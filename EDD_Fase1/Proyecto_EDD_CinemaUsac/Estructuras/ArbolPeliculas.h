#ifndef ARBOLPELICULAS_H
#define ARBOLPELICULAS_H

#include "NodoArbol.h"
#include <iostream>

class ArbolPeliculas {
private:
    NodoArbol* raiz;
    NodoArbol* insertarRecursivo(NodoArbol* nodoActual, Pelicula nuevaPelicula);
    NodoArbol* buscarRecursivo(NodoArbol* nodoActual, int codigoABuscar);
    NodoArbol* eliminarRecursivo(NodoArbol* nodoActual, int codigoAEliminar);
    void inordenRecursivo(NodoArbol* nodoActual);

public:
    ArbolPeliculas();
    void insertar(Pelicula nuevaPelicula);
    void eliminar(int codigoAEliminar);
    Pelicula* buscar(int codigoABuscar);
    void mostrarInorden();
    NodoArbol* getRaiz();  
};

#endif