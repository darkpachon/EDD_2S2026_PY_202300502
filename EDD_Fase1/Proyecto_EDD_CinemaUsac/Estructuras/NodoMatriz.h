#ifndef NODOMATRIZ_H
#define NODOMATRIZ_H

#include <string>
using namespace std;

class NodoMatriz {
public:
    int fila;
    int columna;
    string estado;  
    string cliente; 

    NodoMatriz* arriba;
    NodoMatriz* abajo;
    NodoMatriz* izquierda;
    NodoMatriz* derecha;

    NodoMatriz(int _fila, int _columna, string _estado) {
        fila = _fila;
        columna = _columna;
        estado = _estado;
        cliente = "";
        
        arriba = nullptr;
        abajo = nullptr;
        izquierda = nullptr;
        derecha = nullptr;
    }
};

#endif // NODOMATRIZ_H