#ifndef MATRIZASIENTOS_H
#define MATRIZASIENTOS_H

#include "NodoMatriz.h"
#include <string>

class MatrizAsientos {
private:
    NodoMatriz* raiz;

    NodoMatriz* buscarFila(int fila);
    NodoMatriz* buscarColumna(int columna);
    NodoMatriz* crearFila(int fila);
    NodoMatriz* crearColumna(int columna);

public:
    MatrizAsientos();
    void insertarAsiento(int fila, int columna, std::string cliente);
    NodoMatriz* getRaiz(); // Útil para Graphviz
};

#endif // MATRIZASIENTOS_H