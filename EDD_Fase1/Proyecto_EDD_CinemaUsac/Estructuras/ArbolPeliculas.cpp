#include "ArbolPeliculas.h"

using namespace std;

ArbolPeliculas::ArbolPeliculas() {
    raiz = nullptr;
}

void ArbolPeliculas::insertar(Pelicula nuevaPelicula) {
    raiz = insertarRecursivo(raiz, nuevaPelicula);
}

NodoArbol* ArbolPeliculas::insertarRecursivo(NodoArbol* nodoActual, Pelicula nuevaPelicula) {
    if (nodoActual == nullptr) {
        return new NodoArbol(nuevaPelicula);
    }
    if (nuevaPelicula.codigo < nodoActual->pelicula.codigo) {
        nodoActual->izquierdo = insertarRecursivo(nodoActual->izquierdo, nuevaPelicula);
    } else if (nuevaPelicula.codigo > nodoActual->pelicula.codigo) {
        nodoActual->derecho = insertarRecursivo(nodoActual->derecho, nuevaPelicula);
    }
    return nodoActual;
}

Pelicula* ArbolPeliculas::buscar(int codigoABuscar) {
    NodoArbol* nodoEncontrado = buscarRecursivo(raiz, codigoABuscar);
    if (nodoEncontrado != nullptr) {
        return &(nodoEncontrado->pelicula);
    }
    return nullptr;
}

NodoArbol* ArbolPeliculas::buscarRecursivo(NodoArbol* nodoActual, int codigoABuscar) {
    if (nodoActual == nullptr || nodoActual->pelicula.codigo == codigoABuscar) {
        return nodoActual;
    }
    if (codigoABuscar < nodoActual->pelicula.codigo) {
        return buscarRecursivo(nodoActual->izquierdo, codigoABuscar);
    }
    return buscarRecursivo(nodoActual->derecho, codigoABuscar);
}

void ArbolPeliculas::mostrarInorden() {
    inordenRecursivo(raiz);
    cout << endl;
}

void ArbolPeliculas::inordenRecursivo(NodoArbol* nodoActual) {
    if (nodoActual != nullptr) {
        inordenRecursivo(nodoActual->izquierdo);
        cout << "Cod: " << nodoActual->pelicula.codigo << " - " << nodoActual->pelicula.titulo << " | ";
        inordenRecursivo(nodoActual->derecho);
    }
}

NodoArbol* ArbolPeliculas::getRaiz() {
    return raiz;
}