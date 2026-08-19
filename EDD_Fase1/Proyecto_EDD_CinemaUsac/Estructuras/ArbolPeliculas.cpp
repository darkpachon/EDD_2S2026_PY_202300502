#include "ArbolPeliculas.h"

using namespace std;

ArbolPeliculas::ArbolPeliculas() {
    raiz = nullptr;
}

void ArbolPeliculas::insertar(Pelicula nuevaPelicula) {
    raiz = insertarRecursivo(raiz, nuevaPelicula);
}

void ArbolPeliculas::eliminar(int codigoAEliminar) {
    raiz = eliminarRecursivo(raiz, codigoAEliminar);
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

NodoArbol* ArbolPeliculas::eliminarRecursivo(NodoArbol* nodoActual, int codigoAEliminar) {
    if (nodoActual == nullptr) return nullptr;

    if (codigoAEliminar < nodoActual->pelicula.codigo) {
        nodoActual->izquierdo = eliminarRecursivo(nodoActual->izquierdo, codigoAEliminar);
    } else if (codigoAEliminar > nodoActual->pelicula.codigo) {
        nodoActual->derecho = eliminarRecursivo(nodoActual->derecho, codigoAEliminar);
    } else {
        if (nodoActual->izquierdo == nullptr) {
            NodoArbol* hijoDerecho = nodoActual->derecho;
            delete nodoActual;
            return hijoDerecho;
        }
        if (nodoActual->derecho == nullptr) {
            NodoArbol* hijoIzquierdo = nodoActual->izquierdo;
            delete nodoActual;
            return hijoIzquierdo;
        }

        NodoArbol* sucesor = nodoActual->derecho;
        while (sucesor->izquierdo != nullptr) sucesor = sucesor->izquierdo;
        nodoActual->pelicula = sucesor->pelicula;
        nodoActual->derecho = eliminarRecursivo(nodoActual->derecho, sucesor->pelicula.codigo);
    }

    return nodoActual;
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