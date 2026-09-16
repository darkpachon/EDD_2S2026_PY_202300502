#ifndef ARBOLPELICULAS_H
#define ARBOLPELICULAS_H

#include "NodoArbol.h"
#include <iostream>

// Árbol binario de búsqueda ordenado por código de película.
class ArbolPeliculas {
private:
    NodoArbol* raiz; // Primer nodo del árbol.
    NodoArbol* insertarRecursivo(NodoArbol* nodoActual, Pelicula nuevaPelicula);
    NodoArbol* buscarRecursivo(NodoArbol* nodoActual, int codigoABuscar);
    NodoArbol* eliminarRecursivo(NodoArbol* nodoActual, int codigoAEliminar);
    void inordenRecursivo(NodoArbol* nodoActual);

public:
    ArbolPeliculas(); // Crea un árbol vacío.
    void insertar(Pelicula nuevaPelicula); // Inserta respetando el orden BST.
    void eliminar(int codigoAEliminar); // Elimina una película por código.
    Pelicula* buscar(int codigoABuscar); // Busca una película en el árbol.
    void mostrarInorden(); // Muestra las películas ordenadas.
    NodoArbol* getRaiz(); // Permite generar recorridos y reportes.
};

#endif