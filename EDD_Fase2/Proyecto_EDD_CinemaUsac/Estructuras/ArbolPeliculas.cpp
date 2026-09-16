#include "ArbolPeliculas.h"

using namespace std;

ArbolPeliculas::ArbolPeliculas() {
    // La raíz nula representa un árbol sin películas.
    raiz = nullptr;
}

void ArbolPeliculas::insertar(Pelicula nuevaPelicula) {
    // Se conserva la raíz que devuelve la inserción recursiva.
    raiz = insertarRecursivo(raiz, nuevaPelicula);
}

void ArbolPeliculas::eliminar(int codigoAEliminar) {
    raiz = eliminarRecursivo(raiz, codigoAEliminar);
}

NodoArbol* ArbolPeliculas::insertarRecursivo(NodoArbol* nodoActual, Pelicula nuevaPelicula) {
    // La posición vacía indica dónde debe crearse el nuevo nodo.
    if (nodoActual == nullptr) {
        return new NodoArbol(nuevaPelicula);
    }
    // Los códigos menores van a la izquierda y los mayores a la derecha.
    if (nuevaPelicula.codigo < nodoActual->pelicula.codigo) {
        nodoActual->izquierdo = insertarRecursivo(nodoActual->izquierdo, nuevaPelicula);
    } else if (nuevaPelicula.codigo > nodoActual->pelicula.codigo) {
        nodoActual->derecho = insertarRecursivo(nodoActual->derecho, nuevaPelicula);
    }
    return nodoActual;
}

Pelicula* ArbolPeliculas::buscar(int codigoABuscar) {
    // Se devuelve la dirección del objeto almacenado, evitando copiarlo.
    NodoArbol* nodoEncontrado = buscarRecursivo(raiz, codigoABuscar);
    if (nodoEncontrado != nullptr) {
        return &(nodoEncontrado->pelicula);
    }
    return nullptr;
}

NodoArbol* ArbolPeliculas::buscarRecursivo(NodoArbol* nodoActual, int codigoABuscar) {
    // El orden del BST permite descartar una rama en cada paso.
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
        // Se encontró el nodo y se resuelve según tenga cero, uno o dos hijos.
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

        // Con dos hijos se usa el menor elemento del subárbol derecho.
        NodoArbol* sucesor = nodoActual->derecho;
        while (sucesor->izquierdo != nullptr) sucesor = sucesor->izquierdo;
        nodoActual->pelicula = sucesor->pelicula;
        nodoActual->derecho = eliminarRecursivo(nodoActual->derecho, sucesor->pelicula.codigo);
    }

    return nodoActual;
}

void ArbolPeliculas::mostrarInorden() {
    // Izquierda-raíz-derecha produce el orden ascendente por código.
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