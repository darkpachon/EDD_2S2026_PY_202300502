#ifndef NODOMATRIZ_H
#define NODOMATRIZ_H

#include <string>
using namespace std;

// Nodo usado por las cabeceras y asientos de la matriz dispersa.
class NodoMatriz {
public:
    int fila;
    int columna;
    string estado;          // "Raiz" | "CabeceraFila" | "CabeceraColumna" | "Ocupado"
    string codigoReserva;   // NUEVO: antes era "cliente". Ahora guarda el codigo_reserva.

    NodoMatriz* arriba;
    NodoMatriz* abajo;
    NodoMatriz* izquierda;
    NodoMatriz* derecha;

    NodoMatriz(int _fila, int _columna, string _estado) {
        fila = _fila;
        columna = _columna;
        estado = _estado;
        codigoReserva = "";
        arriba = nullptr;
        abajo = nullptr;
        izquierda = nullptr;
        derecha = nullptr;
    }
};

#endif