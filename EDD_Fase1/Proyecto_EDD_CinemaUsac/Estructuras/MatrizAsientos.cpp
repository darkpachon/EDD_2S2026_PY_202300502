#include "MatrizAsientos.h"

MatrizAsientos::MatrizAsientos() {
    raiz = new NodoMatriz(0, 0, "Raiz");
    filas = 0;
    columnas = 0;
}

void MatrizAsientos::configurar(int nuevasFilas, int nuevasColumnas) {
    if (nuevasFilas <= 0 || nuevasColumnas <= 0) return;
    raiz = new NodoMatriz(0, 0, "Raiz");
    filas = nuevasFilas;
    columnas = nuevasColumnas;
}

void MatrizAsientos::configurarFuncion(std::string peliculaFuncion, std::string horarioFuncion, std::string salaFuncion) {
    pelicula = peliculaFuncion;
    horario = horarioFuncion;
    sala = salaFuncion;
}

void MatrizAsientos::eliminarFuncion() {
    raiz = new NodoMatriz(0, 0, "Raiz");
    filas = 0;
    columnas = 0;
    pelicula.clear();
    horario.clear();
    sala.clear();
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

NodoMatriz* MatrizAsientos::buscarAsiento(int fila, int columna) {
    NodoMatriz* filaActual = buscarFila(fila);
    if (filaActual == nullptr) return nullptr;

    NodoMatriz* asiento = filaActual->derecha;
    while (asiento != nullptr && asiento->columna < columna) asiento = asiento->derecha;
    if (asiento != nullptr && asiento->columna == columna) return asiento;
    return nullptr;
}

bool MatrizAsientos::insertarAsiento(int fila, int columna, std::string cliente) {
    if (filas > 0 && (fila < 1 || fila > filas || columna < 1 || columna > columnas)) return false;
    if (buscarAsiento(fila, columna) != nullptr) return false;

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
    return true;
}

bool MatrizAsientos::eliminarAsiento(int fila, int columna) {
    NodoMatriz* asiento = buscarAsiento(fila, columna);
    if (asiento == nullptr) return false;

    asiento->izquierda->derecha = asiento->derecha;
    if (asiento->derecha != nullptr) asiento->derecha->izquierda = asiento->izquierda;
    asiento->arriba->abajo = asiento->abajo;
    if (asiento->abajo != nullptr) asiento->abajo->arriba = asiento->arriba;
    delete asiento;
    return true;
}

int MatrizAsientos::getFilas() const {
    return filas;
}

int MatrizAsientos::getColumnas() const {
    return columnas;
}

std::string MatrizAsientos::getPelicula() const {
    return pelicula;
}

std::string MatrizAsientos::getHorario() const {
    return horario;
}

std::string MatrizAsientos::getSala() const {
    return sala;
}

NodoMatriz* MatrizAsientos::getRaiz() {
    return raiz;
}