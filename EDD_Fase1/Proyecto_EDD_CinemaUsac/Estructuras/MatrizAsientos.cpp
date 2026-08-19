#include "MatrizAsientos.h"

MatrizAsientos::MatrizAsientos() {
    raiz = new NodoMatriz(0, 0, "Raiz");
}

NodoMatriz* MatrizAsientos::buscarFila(int fila) {
    NodoMatriz* actual = raiz->abajo;
    while (actual != nullptr) {
        if (actual->fila == fila) return actual;
        actual = actual->abajo;
    }
    return nullptr;
}

NodoMatriz* MatrizAsientos::buscarColumna(int columna) {
    NodoMatriz* actual = raiz->derecha;
    while (actual != nullptr) {
        if (actual->columna == columna) return actual;
        actual = actual->derecha;
    }
    return nullptr;
}

NodoMatriz* MatrizAsientos::crearFila(int fila) {
    NodoMatriz* nuevaFila = new NodoMatriz(fila, 0, "CabeceraFila");
    NodoMatriz* actual = raiz;

    while (actual->abajo != nullptr && actual->abajo->fila < fila) {
        actual = actual->abajo;
    }

    nuevaFila->abajo = actual->abajo;
    if (actual->abajo != nullptr) {
        actual->abajo->arriba = nuevaFila;
    }
    nuevaFila->arriba = actual;
    actual->abajo = nuevaFila;

    return nuevaFila;
}

NodoMatriz* MatrizAsientos::crearColumna(int columna) {
    NodoMatriz* nuevaColumna = new NodoMatriz(0, columna, "CabeceraColumna");
    NodoMatriz* actual = raiz;

    while (actual->derecha != nullptr && actual->derecha->columna < columna) {
        actual = actual->derecha;
    }

    nuevaColumna->derecha = actual->derecha;
    if (actual->derecha != nullptr) {
        actual->derecha->izquierda = nuevaColumna;
    }
    nuevaColumna->izquierda = actual;
    actual->derecha = nuevaColumna;

    return nuevaColumna;
}

void MatrizAsientos::insertarAsiento(int fila, int columna, std::string cliente) {
    NodoMatriz* cabeceraFila = buscarFila(fila);
    if (cabeceraFila == nullptr) {
        cabeceraFila = crearFila(fila);
    }

    NodoMatriz* cabeceraCol = buscarColumna(columna);
    if (cabeceraCol == nullptr) {
        cabeceraCol = crearColumna(columna);
    }

    NodoMatriz* nuevoAsiento = new NodoMatriz(fila, columna, "Ocupado");
    nuevoAsiento->cliente = cliente;

    NodoMatriz* actualFila = cabeceraFila;
    while (actualFila->derecha != nullptr && actualFila->derecha->columna < columna) {
        actualFila = actualFila->derecha;
    }
    nuevoAsiento->derecha = actualFila->derecha;
    if (actualFila->derecha != nullptr) {
        actualFila->derecha->izquierda = nuevoAsiento;
    }
    nuevoAsiento->izquierda = actualFila;
    actualFila->derecha = nuevoAsiento;

    NodoMatriz* actualCol = cabeceraCol;
    while (actualCol->abajo != nullptr && actualCol->abajo->fila < fila) {
        actualCol = actualCol->abajo;
    }
    nuevoAsiento->abajo = actualCol->abajo;
    if (actualCol->abajo != nullptr) {
        actualCol->abajo->arriba = nuevoAsiento;
    }
    nuevoAsiento->arriba = actualCol;
    actualCol->abajo = nuevoAsiento;
}

NodoMatriz* MatrizAsientos::getRaiz() {
    return raiz;
}